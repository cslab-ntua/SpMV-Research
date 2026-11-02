#!/usr/bin/env python3
"""
Log parsing utility for SpMV benchmark outputs.

Scans benchmark log directories, parses .out output files, and aggregates runs
into unified DataFrames exported to partitioning/CSV/parsed_<source>.csv.
"""

import argparse
import os
import re
import sys

import numpy as np
import pandas as pd
from scipy.stats import hmean

# Strategy mapping from filename tokens to canonical names
STRATEGY_MAP = {
    'RATIO': 'NAIVE',
    'MIN_X_ACCESS_CPU': 'MIN-X',
}

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
BASE_DIR = os.path.abspath(os.path.join(SCRIPT_DIR, '..'))

LOG_DIRS = {
    'baseline':            os.path.join(BASE_DIR, 'out_logs'),
    'col_reordering':      os.path.join(BASE_DIR, 'out_logs_COL_REORDERING'),
    'cpu_local_x_opt':     os.path.join(BASE_DIR, 'out_logs_CPU_LOCAL_X_OPT'),
    'baseline_rcm':        os.path.join(BASE_DIR, 'out_logs_RCM'),
    'cpu_local_x_opt_rcm': os.path.join(BASE_DIR, 'out_logs_CPU_LOCAL_X_OPT_RCM'),
}

CSV_DIR = os.path.join(SCRIPT_DIR, 'partitioning', 'CSV')
MATRIX_ORDER_TXT = os.path.join(SCRIPT_DIR, 'matrix_order.txt')
MATRIX_FEATURES_CSV = os.path.join(SCRIPT_DIR, 'matrix_features.csv')


def _nan_hmean(values):
    v = pd.Series(values).dropna()
    v = v[v > 0]
    return hmean(v) if len(v) else np.nan


def _parse_filename(filename, known_gpu_kernels, known_cpu_kernels):
    """Return a dict describing the run encoded in a log filename, or None."""
    if filename.startswith('hybrid_'):
        m = re.match(r'^hybrid_(.*?)_STRAT_(.*?)_(\d+)_(EXPLICIT|MALLOC)_nv_d\.out$', filename)
        if not m:
            return None
        kernels_part, strategy_raw, ratio, impl_type = m.group(1), m.group(2), int(m.group(3)), m.group(4)
        cpu_kernel = gpu_kernel = None
        for gk in known_gpu_kernels:
            if kernels_part.endswith('_' + gk):
                gpu_kernel, cpu_kernel = gk, kernels_part[:-len('_' + gk)]
                break
        if gpu_kernel is None:
            for ck in known_cpu_kernels:
                if kernels_part.startswith(ck + '_'):
                    cpu_kernel, gpu_kernel = ck, kernels_part[len(ck + '_'):]
                    break
        if gpu_kernel is None:
            for ck in ['div_rf', 'sell_sorted_csr', 'sell_sorted', 'csr_vec', 'armpl']:
                if kernels_part.startswith(ck + '_'):
                    cpu_kernel, gpu_kernel = ck, kernels_part[len(ck + '_'):]
                    break
        if gpu_kernel is None:
            parts = kernels_part.split('_', 1)
            cpu_kernel, gpu_kernel = (parts[0], parts[1]) if len(parts) == 2 else (kernels_part, 'unknown')
        return {
            'is_hybrid': True,
            'is_cpu_standalone': False,
            'cpu_kernel': cpu_kernel,
            'gpu_kernel': gpu_kernel,
            'strategy': STRATEGY_MAP.get(strategy_raw, strategy_raw),
            'ratio': ratio,
            'impl_type': impl_type
        }

    m_gpu = re.match(r'^(.*?)_(EXPLICIT|MALLOC)_nv_d\.out$', filename)
    if m_gpu:
        return {
            'is_hybrid': False,
            'is_cpu_standalone': False,
            'cpu_kernel': None,
            'gpu_kernel': m_gpu.group(1),
            'strategy': 'NONE',
            'ratio': None,
            'impl_type': m_gpu.group(2)
        }
    m_cpu = re.match(r'^(.*?)_d\.out$', filename)
    if m_cpu:
        return {
            'is_hybrid': False,
            'is_cpu_standalone': True,
            'cpu_kernel': m_cpu.group(1),
            'gpu_kernel': None,
            'strategy': 'NONE',
            'ratio': None,
            'impl_type': 'CPU_ONLY'
        }
    return None


def _parse_block(block, info):
    """Extract the metrics of one 'File: ...' block. Returns None if no GFLOPS line."""
    is_hybrid, is_cpu = info['is_hybrid'], info['is_cpu_standalone']
    rec = {
        'GFLOPS': None, 'CPU_GFLOPS': None, 'GPU_GFLOPS': None,
        'Time_ms': None, 'CPU_Time_ms': None, 'GPU_Time_ms': None,
        'Kernel': None, 'm_cpu': None, 'nnz_cpu': None, 'n_cpu': None,
        'm_gpu': None, 'nnz_gpu': None, 'n_gpu': None
    }

    for part, key_dim, key_n, key_nnz in [('CPU', 'm_cpu', 'n_cpu', 'nnz_cpu'), ('GPU', 'm_gpu', 'n_gpu', 'nnz_gpu')]:
        m = re.search(part + r' Portion.*?:\s+(?P<dim>\d+)\s+(?P<unit>rows|cols).*?,?\s+(?P<nnz>\d+)\s+NNZs', block)
        if m:
            rec[key_dim if m.group('unit') == 'rows' else key_n] = int(m.group('dim'))
            rec[key_nnz] = int(m.group('nnz'))

    g = re.search(r'GFLOPS = (?P<val>[\d\.]+)(?:\s*\((?P<kernel>[^\)]+)\))?', block)
    if not g:
        return None
    rec['GFLOPS'] = float(g.group('val'))
    rec['Kernel'] = g.group('kernel')

    t = re.search(r'time = (?P<val>[\d\.]+) ms', block)
    if t:
        rec['Time_ms'] = float(t.group('val'))

    c = re.search(r'CPU Part: (?P<val>[\d\.]+) GFLOPS', block)
    rec['CPU_GFLOPS'] = float(c.group('val')) if c else (rec['GFLOPS'] if is_cpu else None)
    gp = re.search(r'GPU Part: (?P<val>[\d\.]+) GFLOPS', block)
    rec['GPU_GFLOPS'] = float(gp.group('val')) if gp else (rec['GFLOPS'] if (not is_hybrid and not is_cpu) else None)

    ct = re.search(r'CPU Part:.*?time median: (?P<val>[\d\.]+) ms', block)
    rec['CPU_Time_ms'] = float(ct.group('val')) if ct else (rec['Time_ms'] if is_cpu else None)
    gt = re.search(r'GPU Part:.*?time median: (?P<val>[\d\.]+) ms', block)
    rec['GPU_Time_ms'] = float(gt.group('val')) if gt else (rec['Time_ms'] if (not is_hybrid and not is_cpu) else None)

    pad = re.search(r'\(padding:?\s+([\d\.]+)%?\)', block)
    rec['Padding_Pct'] = float(pad.group(1)) if pad else np.nan

    gather = re.search(r'CPU needs (?P<elems>\d+) elements \((?P<mb_gather>[\d\.]+) MB\) instead of (?P<mb_total>[\d\.]+) MB', block)
    if gather:
        total_mb = float(gather.group('mb_total'))
        rec['Gather_Elems'] = int(gather.group('elems'))
        rec['Gather_Pct'] = float(gather.group('mb_gather')) / total_mb * 100.0 if total_mb > 0 else 0.0
    else:
        rec['Gather_Elems'] = np.nan
        rec['Gather_Pct'] = np.nan

    if rec['Kernel'] is None:
        if is_hybrid:
            rec['Kernel'] = f"Hybrid_{info['cpu_kernel']}_{info['gpu_kernel']}"
        else:
            rec['Kernel'] = info['cpu_kernel'] if is_cpu else info['gpu_kernel']
    return rec


def discover_standalone_kernels(log_dirs):
    """Scan all log directories for standalone .out files to discover known CPU and GPU kernel names."""
    known_gpu, known_cpu = set(), set()
    for log_dir in log_dirs.values():
        if not os.path.isdir(log_dir):
            continue
        run_dirs = [d for d in os.listdir(log_dir) if os.path.isdir(os.path.join(log_dir, d)) and d.startswith('run')]
        if not run_dirs:
            run_dirs = ['.']
        for run_name in run_dirs:
            run_path = os.path.join(log_dir, run_name)
            if not os.path.isdir(run_path):
                continue
            for filename in os.listdir(run_path):
                if filename.startswith('hybrid_') or not filename.endswith('.out'):
                    continue
                m_gpu = re.match(r'^(.*?)_(EXPLICIT|MALLOC)_nv_d\.out$', filename)
                if m_gpu:
                    known_gpu.add(m_gpu.group(1))
                elif re.match(r'^(.*?)_d\.out$', filename):
                    known_cpu.add(filename[:-len('_d.out')])
    return known_gpu, known_cpu


def parse_logs(log_dir, known_gpu=None, known_cpu=None):
    """
    Read every .out file under <log_dir>/run*/ and return (df, matrix_order).
    Rows of the same configuration across run* directories are aggregated
    (hmean for GFLOPS columns, mean for the rest).
    """
    if not os.path.isdir(log_dir):
        print(f"  Directory {log_dir} does not exist.")
        return pd.DataFrame(), []

    run_dirs = sorted(d for d in os.listdir(log_dir) if os.path.isdir(os.path.join(log_dir, d)) and d.startswith('run'))
    if not run_dirs:
        run_dirs = ['.']
    print(f"  Parsing run directories: {run_dirs}")

    disc_gpu, disc_cpu = set(), set()
    for run_name in run_dirs:
        run_path = os.path.join(log_dir, run_name)
        if not os.path.isdir(run_path):
            continue
        for filename in os.listdir(run_path):
            if filename.startswith('hybrid_') or not filename.endswith('.out'):
                continue
            m_gpu = re.match(r'^(.*?)_(EXPLICIT|MALLOC)_nv_d\.out$', filename)
            if m_gpu:
                disc_gpu.add(m_gpu.group(1))
            elif re.match(r'^(.*?)_d\.out$', filename):
                disc_cpu.add(filename[:-len('_d.out')])

    effective_gpu = set(known_gpu) if known_gpu else disc_gpu
    effective_cpu = set(known_cpu) if known_cpu else disc_cpu

    all_data, matrix_order = [], []
    for run_name in run_dirs:
        run_path = os.path.join(log_dir, run_name)
        for filename in sorted(os.listdir(run_path)):
            if not filename.endswith('.out'):
                continue
            info = _parse_filename(filename, effective_gpu, effective_cpu)
            if info is None:
                continue
            with open(os.path.join(run_path, filename), 'r') as f:
                content = f.read()
            for block in re.split(r'^File:', content, flags=re.MULTILINE):
                if not block.strip():
                    continue
                first_line = block.split('\n', 1)[0]
                m_path = re.search(r'^.*\.mtx', first_line)
                if not m_path:
                    continue
                matrix_name = os.path.basename(m_path.group(0)).replace('.mtx', '')
                if matrix_name not in matrix_order:
                    matrix_order.append(matrix_name)
                rec = _parse_block(block, info)
                if rec is None:
                    continue
                label = 'Hybrid' if info['is_hybrid'] else 'Standalone'
                if info['ratio']:
                    label += f"_{info['ratio']}"
                rec.update({
                    'Matrix': matrix_name,
                    'Strategy': info['strategy'],
                    'Type': info['impl_type'],
                    'Ratio': info['ratio'],
                    'IsHybrid': info['is_hybrid'],
                    'Label': label,
                    'Run': run_name,
                    'CPU_Kernel': info['cpu_kernel'],
                    'GPU_Kernel': info['gpu_kernel']
                })
                all_data.append(rec)

    if not all_data:
        return pd.DataFrame(), matrix_order

    raw = pd.DataFrame(all_data)
    agg = {col: _nan_hmean for col in ['GFLOPS', 'CPU_GFLOPS', 'GPU_GFLOPS']}
    for col in ['Time_ms', 'CPU_Time_ms', 'GPU_Time_ms', 'm_cpu', 'nnz_cpu', 'n_cpu', 'm_gpu', 'nnz_gpu', 'n_gpu', 'Padding_Pct', 'Gather_Elems', 'Gather_Pct']:
        agg[col] = 'mean'
    group_cols = ['Matrix', 'Type', 'Ratio', 'IsHybrid', 'Label', 'Kernel', 'CPU_Kernel', 'GPU_Kernel', 'Strategy']
    df = raw.groupby(group_cols, dropna=False, observed=True).agg(agg).reset_index()

    df['avg_nnz_cpu'] = np.where(df['m_cpu'] > 0, df['nnz_cpu'] / df['m_cpu'], 0)
    df['avg_nnz_gpu'] = np.where(df['m_gpu'] > 0, df['nnz_gpu'] / df['m_gpu'], 0)
    df['GPU_CPU_Time_Ratio'] = np.where((df['IsHybrid'] == True) & (df['CPU_Time_ms'] > 0), df['GPU_Time_ms'] / df['CPU_Time_ms'], np.nan)
    return df, matrix_order


def main():
    parser = argparse.ArgumentParser(description="Parse SpMV raw benchmark logs into parsed CSVs.")
    parser.add_argument('--log-source', default=None, help="specific log source to parse (e.g. 'baseline', 'baseline_rcm'). Default: all")
    parser.add_argument('--csv-dir', default=CSV_DIR, help=f"directory to save parsed CSVs (default: {CSV_DIR})")
    args = parser.parse_args()

    os.makedirs(args.csv_dir, exist_ok=True)
    sources = {args.log_source: LOG_DIRS[args.log_source]} if args.log_source and args.log_source in LOG_DIRS else LOG_DIRS

    global_known_gpu, global_known_cpu = discover_standalone_kernels(sources)
    for name, path in sources.items():
        print(f"\n--- Parsing [{name}]: {path} ---")
        df, _ = parse_logs(path, known_gpu=global_known_gpu, known_cpu=global_known_cpu)
        if not df.empty:
            out_file = os.path.join(args.csv_dir, f'parsed_{name}.csv')
            df.to_csv(out_file, index=False)
            print(f"  Saved {len(df)} records to {out_file}")


if __name__ == '__main__':
    main()

