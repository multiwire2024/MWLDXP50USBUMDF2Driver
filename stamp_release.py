#/bin/python3

from datetime import datetime

v = open("_version.h", "r")
cur_time = datetime.utcnow()
lines = v.readlines()
for i, line in enumerate(lines):
    if "MWLUSB_BUILD_VERSION" in line:
        datestr = cur_time.strftime("%y%m%d%H")
        lines[i] = "#define MWLUSB_BUILD_VERSION    "+datestr

v = open("version.h.new", "w")
v.writelines(lines)
