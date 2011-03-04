__all__ = ['make_timestamp_folder']

import os, time

'''
This python script requires Python 2.6 or 2.7
'''

def make_timestamp_folder():
    base_foldername = raw_input("What is the base log folder name?\n==>")
    x = os.path.join(base_foldername, time.strftime("%Y%m%d.%H.%M.%S"))
    os.mkdir(x)

if __name__ == "__main__":
    make_timestamp_folder()
