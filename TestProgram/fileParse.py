from gettext import find


orignal = open("C:\\Users\\Matthew Champagne\\OneDrive\\Personal Projects\\MIPS-Disassembler\\TestProgram\\mipsNoPseudo.asm")
disassembled = open("C:\\Users\\Matthew Champagne\\OneDrive\\Personal Projects\\MIPS-Disassembler\\Source\\Output.asm")

flag = True

orignalLines = orignal.readlines()
disLines = disassembled.readlines()
extra = 0

for i in range(len(disLines)):
    origStr = orignalLines[i + extra].strip().replace(',', ' ').replace('(', ' ').replace(')', ' ').split()
    disStr = disLines[i].strip().replace(',', ' ').replace('(', ' ').replace(')', ' ').split()
    
    for j in range(len(disStr)):
        if(disStr[j] != origStr[j]):
            print("Line " + str(i + 1) + "- " + disLines[i].strip() + " <-> " + orignalLines[i + extra].strip())
            flag = False
            break

if flag:
    print("The files match")
