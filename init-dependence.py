#! /usr/bin/env python3
import os
import os.path
import subprocess


SCRIPT_DIR = os.path.abspath(os.path.dirname(__file__))


def main():
    cmockery_dir = os.path.join(SCRIPT_DIR, 'third-party', 'cmockery')
    libcmockery_dir = os.path.join(SCRIPT_DIR, 'third-party', 'libcmockery')

    commands = [
        (['git', 'clone', 'https://github.com/google/cmockery', cmockery_dir],
         None, lambda: not os.path.exists(cmockery_dir)),
        (['./configure'], cmockery_dir, None),
        (['make'], cmockery_dir, None),
        (['make', 'install', 'prefix=' + libcmockery_dir], cmockery_dir, None)
    ]

    for cmd, cwd, cond in commands:
        if not cond or cond():
            check_call(cmd, cwd=cwd)


def check_call(*args, **kwargs):
    p = subprocess.Popen(*args, **kwargs)
    try:
        r = p.wait()
    except Exception as e:
        p.terminate()
        raise e

    if r != 0:
        raise subprocess.CalledProcessError()


if __name__ == '__main__':
    main()
