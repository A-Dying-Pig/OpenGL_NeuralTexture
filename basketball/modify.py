

with open("NBA BASKETBALL.obj","r") as fp:
    with open("basketball.obj","w") as fw:
        for ln in fp:
            if ln.startswith("vt"):
                ln = ln.split(" ")
                del ln[-1]
                ln = " ".join(ln) + "\n"
            fw.write(ln)

