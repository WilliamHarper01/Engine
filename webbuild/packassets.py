def compile_assets(dir, write_path):
    writer = FileWriter(write_path)

    for root, directories, files in os.walk(dir):
        for f in files:
            split = f.split(".")
            name = split[0]
            ext = split[1]
            path = os.path.join(root, f)

            if ext == 'glsl': writer.glsl(name, path)
            elif ext == 'png': writer.png(name, path)
            #etc

    writer.i32(ASSET_TYPE_END)
    writer.close()