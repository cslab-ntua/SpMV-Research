#!/bin/bash
BINLIB=$1
TUNED=$2
THRDS=$3
MAT_DIR=$4
SPD_MAT_DIR=$5

export OMP_NUM_THREADS=$THRDS
export MKL_NUM_THREADS=$THRDS

header=1

MATS=($(ls  $MAT_DIR))
SPD_MATS=($(ls $SPD_MAT_DIR))

if [ "$TUNED" ==  1 ]; then
  for bp in {0,1}; do
    for mat in "${SPD_MATS[@]}"; do
      for k in {2,3,4,5,6,7,8,9}; do
        # for cparm in {1,2,3,4,5,10,20}; do
        if [ $header -eq 1 ]; then
          $BINLIB  -m $SPD_MAT_DIR/$mat/$mat.mtx -n SPTRS -s CSR -t $THRDS -c $k -d -p $bp -u 1 --analyze
          header=0
        else
          $BINLIB  -m $SPD_MAT_DIR/$mat/$mat.mtx -n SPTRS -s CSR -t $THRDS -c $k -p $bp -u 1 --analyze
        fi
      done
    done
  done
fi

prefer_fsc=( "" )
clt_min_widths=( 2 4 8 16 )
clt_max_distances=( 2 4 8 )

### SPMV
if [ "$TUNED" == 3 ]; then
  for mat in "${MATS[@]}"; do
    for md in "${clt_max_distances[@]}"; do
      for mw in "${clt_min_widths[@]}"; do
        for pf in "${prefer_fsc[@]}"; do
          if [ $header -eq 1 ]; then
            $BINLIB  -m $MAT_DIR/$mat/$mat.mtx -n SPMV -s CSR -t $THRDS $pf --clt_width=$mw --col_th=$md -d --analyze
            header=0
          else
            $BINLIB  -m $MAT_DIR/$mat/$mat.mtx -n SPMV -s CSR -t $THRDS $pf --clt_width=$mw --col_th=$md --analyze
          fi
        done
      done
    done
  done
fi

M_TILE_SIZES=( 4 8 16 32 )
N_TILE_SIZES=( 4 8 16 32 )
B_MAT_COL=( 4 16 64 128 )
### SPMM
if [ "$TUNED" == 4 ]; then
  for mat in "${MATS[@]}"; do
    for md in "${clt_max_distances[@]}"; do
      for mw in "${clt_min_widths[@]}"; do
        for mtile in "${M_TILE_SIZES[@]}"; do
          for ntile in "${N_TILE_SIZES[@]}"; do
            for bcol in "${B_MAT_COL[@]}"; do
              if [ "$ntile" -gt "$bcol" ]; then
                continue
              fi
              if [ $header -eq 1 ]; then
                $BINLIB -m $MAT_DIR/$mat/$mat.mtx -n SPMM -s CSR -t $THRDS --m_tile_size=$mtile --n_tile_size=$ntile --b_matrix_columns=$bcol -d --clt_width=$mw --col_th=$md --analyze
                header=0
              else
                $BINLIB -m $MAT_DIR/$mat/$mat.mtx -n SPMM -s CSR -t $THRDS --b_matrix_columns=$bcol --m_tile_size=$mtile --n_tile_size=$ntile --clt_width=$mw --col_th=$md --analyze
              fi
            done
          done
        done
      done
    done
  done
fi
