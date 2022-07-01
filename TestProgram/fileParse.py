orignal = open("C:\\Users\\Matthew Champagne\\OneDrive\\Personal Projects\\MIPS-Disassembler\\TestProgram\\mipsNoLabels.asm")
disassembled = open("C:\\Users\\Matthew Champagne\\OneDrive\\Personal Projects\\MIPS-Disassembler\\Source\\Output.asm")

flag = True

orignalLines = orignal.readlines()
disLines = disassembled.readlines()

for i in range(len(orignalLines)):
    origStr = orignalLines[i].strip().replace(',', ' ').replace('(', ' ').replace(')', ' ').split()
    disStr = disLines[i].strip().replace(',', ' ').replace('(', ' ').replace(')', ' ').split()
    
    if(disStr[0] != origStr[0]):
        print("Line " + str(i) + "- " + disStr[0] + " line " + str(i) + "- " + origStr[0])
        flag = False

if flag:
    print("The files match")
