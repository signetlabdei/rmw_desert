import subprocess
from os import listdir
from os.path import isfile, join

src_path = "src"
header_path = "include/rmw"
header_files = [f for f in listdir(header_path) if isfile(join(header_path, f))]

for header_file in header_files:
    src_full_path = join(src_path, header_file.replace(".h", ".c"))
    header_full_path = join(header_path, header_file)

    source = ""

    if isfile(src_full_path):
        with open(src_full_path, "r") as source_file:
            source = source_file.read()

    ctags = subprocess.run(['ctags', '-x', '--c-kinds=p', '--_xformat="%-16N %4n %-16F %C %S"', header_full_path], stdout=subprocess.PIPE)
    ctag_lines = ctags.stdout.decode("utf-8").split("\n")

    header_functions = []
    header_prototypes = []
    
    included = False

    for line in ctag_lines:
        if line == "": continue
        function = line.split(" ")[0].replace("\"", "")
        prototype = "rmw_ret_t " + line.split(".h ", 1)[1].replace("( (", "(").split(")", 1)[0] + ")"
        
        if not function in source or not isfile(src_full_path):
            with open("gen/" + header_file.replace(".h", ".cpp"), "a") as out:
                if not included:
                    out.write("#include \"rmw/" + header_file + "\"\n\n\n")
                    included = True
                    print("  src/" + header_file.replace(".h", ".cpp"))
                implementation = prototype + "\n{\n  return RMW_RET_OK;\n}\n\n"
                out.write(implementation)
