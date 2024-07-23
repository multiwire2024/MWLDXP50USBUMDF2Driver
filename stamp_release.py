#/bin/python3

from datetime import datetime

v = open("version.h", "r")
cur_time = datetime.utcnow()
lines = v.readlines()
for i, line in enumerate(lines):
    if "MWLUSB_MINOR_VERSION" in line:
        datestr = cur_time.strftime("%Y")
        lines[i] = "#define MWLUSB_MINOR_VERSION    "+datestr+"\n"
    elif "MWLUSB_BUILD_VERSION" in line:
        datestr = cur_time.strftime("%m%d")
        lines[i] = "#define MWLUSB_BUILD_VERSION    "+datestr

v = open("version.h.new", "w")
v.writelines(lines)
