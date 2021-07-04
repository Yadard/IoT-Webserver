import os

exclude = ["\t", '\r', '\n', ' ']

def compress(filepath: string):
    with open(filepath, 'r') as f:
        content = f.read(-1)
    folder_path = filepath.split('/')[-1] = "compress"
    file_path = os.path.join(folder_path, filepath.split('/')[-1])

    compress = ""
    for c in content:
        if c not in exclude:
            compress += c

    if not os.path.exists(file_path):
        os.mkdir(folder_path)
        
    with open(file_path, 'w') as c:
        c.write


def getFiles():
    current_folder = os.getcwd()
    files = [f for f in os.listdir(current_folder) if os.path.isFile(os.path.join(current_folder, f))]
    return files

def main():
    files = getFiles()
    #for f in files:
    #    compress(f):