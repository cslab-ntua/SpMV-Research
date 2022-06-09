#!/usr/bin/env python3


""" Script to format all the C/C++ and python files in the main git """

#
# Requirements:
#
# sudo  apt-get  install  dos2unix  python-autopep8  clang-format-3.8
#
import os
import subprocess


# pylint: disable=redefined-outer-name, invalid-name

def execute_cmd(cmd, working_dir=None, verbose=True):
    """ Execute a command, with verbosity and return status """

    ret = True
    try:
        if verbose:
            if working_dir is not None:
                print("\n>> " + cmd + "   (from: " + working_dir + ")\n")
            else:
                print("\n>> " + cmd + "\n")
        proc = subprocess.Popen(cmd.split(" "), cwd=working_dir,
                                stdout=subprocess.PIPE)
        stdout, stderr = proc.communicate()
        lines = stdout.decode('UTF-8').split('\n')
        if stderr:
            lines += "stderr:"
            lines += stderr.decode('UTF-8').split('\n')
        if proc.returncode != 0:
            if verbose:
                print("Command return code " + str(proc.returncode))
            ret = False
        if verbose:
            for line in lines:
                print(line)
        return ret, lines
    except OSError as exception:
        print(exception)
        ret = False
    except subprocess.TimeoutExpired as timeout:
        print(timeout)
        ret = False
    return ret, []


def coding_style(filename, filter_ext=None):
    """ Format the input file according to coding style """

    extensions_cpp = [".cpp", ".hpp", ".c", ".h", ".cc", ".hxx", ".cu"]
    extensions_py = [".py"]

    if not os.path.isfile(filename):
        return

    if filename.find('generated') != -1:
        return

    if (filter_ext is None) or (filter_ext == "cpp"):
        for ext in extensions_cpp:
            if (filename.endswith(ext) and (not filename.startswith('inc/cusplibrary-0.5.1/'))):
                execute_cmd("dos2unix " + filename)
                execute_cmd("clang-format-3.8 -i -style=Google " + filename)
                return

    if (filter_ext is None) or (filter_ext == "py"):
        for ext in extensions_py:
            if filename.endswith(ext):
                execute_cmd("dos2unix " + filename)
                execute_cmd("autopep8 -i -a --max-line-length=100 " + filename)
                return
    return


if __name__ == "__main__":

    #ret, lines = execute_cmd("git ls-files")
    ret, lines = execute_cmd("git diff origin/master --name-only")
    for filename in lines:
        coding_style(filename)
