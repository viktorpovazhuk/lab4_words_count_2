from re import T
import os
import subprocess
import matplotlib.pyplot as plt

subprocess.call(["mkdir CmakeBuildDir"], shell=True)
os.chdir(os.getcwd() + '/CmakeBuildDir')
#subprocess.call(["cd CmakeBuildDir"], shell=True)
subprocess.call(["cmake ~./.."], shell=True)
subprocess.call(["make"], shell=True)
#subprocess.call([""], shell=True)

#subprocess.call("a.exe ./config.cfg", shell=True)

#p = subprocess.Popen(["a.exe", "./config.cfg"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

#out, err = p.communicate()
#out = out.decode()

num = 1
results = []
ns = []
min = -1
min_n = 0
for i in range(20):
    ns.append(num)
    f = open("../configs/index.cfg", "r")
    str_file = ""
    lines = f.readlines()
    for line in lines:
        if (line != '\n'):
            if (line.split("=")[0] == "indexing_threads"):
                line = "indexing_threads=" + str(num) + "\n"
                num += 1
        str_file += line

    str_file = str_file.strip()
    f.close()
    f = open("../configs/index.cfg", "w")
    f.write(str_file)
    f.close()

    p = subprocess.Popen(["./counter"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    out, err = p.communicate()
    out = out.decode()

    try:
        n = int(out.split('\n')[0].split("=")[1])
    except ValueError:
        print("Error occured")
        print(out)
        exit()
    if (min == -1):
        min = n
        min_n = num-1
    elif (min > n):
        min = n
        min_n = num-1
    
    results.append(n)

print("Minimal time: ", min)
print("Number of threads for minimal time", min_n)

plt.plot(ns, results)
plt.xlabel('number of threads')
plt.ylabel('time in microseconds')
plt.title('graph')
plt.show()


