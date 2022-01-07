from pcbnew import *
import math

board = GetBoard()
io = PCB_IO()

# Draw the outline of the board, with the connection-tabs
boardSize = [44.142, 60.00] # In mm

originalR = FootprintLoad('/Library/Application Support/kicad/modules/Resistor_SMD.pretty', "R_0603_1608Metric")

# For layer IDs: Definition at line 65 of file layer_ids.h.
def drawEdgeLine(xStart, yStart, xEnd, yEnd):
    seg = PCB_SHAPE(board)
    board.Add(seg)
    seg.SetStart(wxPoint(FromMM(xStart), FromMM(yStart)))
    seg.SetEnd(wxPoint(FromMM(xEnd), FromMM(yEnd)))
    seg.SetLayer(Edge_Cuts)

tabPositions = []
def drawOutlines():
    bottomWidthInc = 24.853
    drawEdgeLine(0, 0, -bottomWidthInc, boardSize[1])
    drawEdgeLine(boardSize[0], 0, boardSize[0] + bottomWidthInc, boardSize[1])
    drawEdgeLine(-bottomWidthInc, boardSize[1], boardSize[0] + bottomWidthInc, boardSize[1])

    numTabs = 9
    tabWidth = 1.6
    tabSpacing = 4
    for i in range(numTabs):
        index = i - int(numTabs / 2) # -2 -1 0 1 2
        center = boardSize[0] * 0.5
        xStart = center - (tabWidth * 0.5)
        xEnd = center + (tabWidth * 0.5)
        yStart = 0 - tabWidth
        yEnd = 0
        xStart -= (tabSpacing * index) # Offset by index, so they get spaced on X-axis
        xEnd -= (tabSpacing * index)
        drawEdgeLine(xStart, yStart, xEnd, yStart)
        drawEdgeLine(xStart, yStart, xStart, yEnd)
        drawEdgeLine(xEnd, yStart, xEnd, yEnd)
        tabPositions.append([center - (tabSpacing * index), yStart])
        if i != numTabs -1: # Connect the tabs
            nextX = center + (tabWidth * 0.5) - (tabSpacing * (index + 1))
            drawEdgeLine(xStart, yEnd, nextX, yEnd)
        if i == 0: # Connect left tab to left corner of board
            drawEdgeLine(xEnd, yEnd, boardSize[0], yEnd)
        if i == numTabs -1: # Right to right
            drawEdgeLine(0, yEnd, xStart, yEnd)

def placeHolesOnTabs(positions):
    numHoles = 0
    for p in positions:
        footprint_lib = '/Users/jildertviet/Documents/kiCadLibs/jildertTest.pretty'
        mod = io.FootprintLoad(footprint_lib, 'jCastellatedHole')
        mod.SetPosition(wxPoint(FromMM(p[0]), FromMM(p[1] - 0.15)))
        # mod.SetValue('')
        pName = 'p' + str(numHoles)
        mod.SetReference(pName)
        mod.Reference().SetVisible(False)
        board.Add(mod)
        net = ['12V', '', 'R', 'G', '12V', 'B', 'W', '', '12V'][numHoles]
        if net != '':
            connectPinToNet(pName, 0, net)

        numHoles += 1

def placeR(pos, name, rotation=0):
    r = FOOTPRINT(originalR)
    r.SetPosition(wxPoint(FromMM(pos[0]), FromMM(pos[1])))
    r.Rotate(r.GetPosition(), (rotation) * 10)
    # mod.SetValue('')
    r.SetReference(name)
    r.Reference().SetVisible(False)
    board.Add(r)

def connectPinToNet(componentName, pinIndex, netName):
    r = board.FindFootprintByReference(componentName)
    # print(r)
    net = board.GetNetsByName().has_key(netName)
    print(net)
    net = board.GetNetsByName().find(netName).value()[1]
    print(net)
    # print(vin)
    # print(len(r.Pads()))
    # if pinIndex >= len(r.Pads()):
        # print('x')
    # return 0
    r.Pads()[pinIndex].SetNet(net)

def createNet(netName):
    net = NETINFO_ITEM(board, netName, len(board.GetNetsByName()))
    board.Add(net)

colPerRow = [3,4,5,6,6,7] #31 total

def addFootPrints():
    index = 0
    spacing = boardSize[0] / 4
    i = 0
    numRGB = 0
    numWhite = 0
    numRWhite = 0
    numR = 0
    numRRGB = 0
    ledRotation = 0
    for y in colPerRow:
        for x in range(y):
            if y % 2 != 0:
                indexT = (x - int(y * 0.5))
            else:
                indexT = (x+0.5) - int(y * 0.5)
            if i % 2 == 1:
                indexT *= -1
                ledRotation = 180
            else:
                ledRotation = 0
            xCenter = indexT * spacing + (boardSize[0] * 0.5)
            yCenter = (boardSize[1] / len(colPerRow)) * (i + 0.5)
            # print(str(xCenter) + ', ' + str(yCenter))

            footprint_lib = '/Library/Application Support/kicad/modules/LED_SMD.pretty'
            bWhite = False

            if index % 2 == 0: # WHITE
                bWhite = True
                # mod = io.FootprintLoad(footprint_lib, 'LED_PLCC-2')
                footprintName = 'LED_RGB_5050-6'
                if numWhite % 2 == 0: # Place a R for 2 white LEDs
                    rName = 'RW' + str(numRWhite)
                    placeR([xCenter, yCenter + 4], rName, 180 - ledRotation)
                    numR += 1
                    numRWhite += 1
                    connectPinToNet(rName, 0, '12V')

                    netName = 'rWhite' + str(numRWhite) #Create new net for the led-R
                    createNet(netName)
                    connectPinToNet(rName, 1, netName)

                # Increase counter numWhite += 1 later!
            else:
                footprintName = 'LED_RGB_5050-6' # RGB
                rName = 'R' + str(numRRGB)
                if numRGB % 3 == 0: # Should place all the R @ first LED
                    index_ = 0
                    for offset in [[5.5, -2], [5.5, -4], [5.5, -6]]:
                        rName = 'R' + str(numRRGB)
                        placeR([xCenter - offset[0], yCenter + 4 + offset[1]], rName)
                        connectPinToNet(rName, 0, '12V')
                        # netName = ['rGreen', 'rRed', 'rBlue'][numRGB % 3] + str(int(numRGB / 3)) # rRed0, rRed1, etc
                        # createNet(netName)
                        # connectPinToNet(rName, 1, netName)

                        index_ += 1
                        numR += 1
                        numRRGB += 1


            mod = FootprintLoad('/Library/Application Support/kicad/modules/LED_SMD.pretty', footprintName)
            dName = 'D' + str(index)
            mod.SetReference(dName)
            if ledRotation != 0:
                mod.Rotate(mod.GetPosition(), (ledRotation) * 10)
            mod.SetPosition(wxPoint(FromMM(xCenter), FromMM(yCenter)))
            mod.Reference().SetVisible(False)
            board.Add(mod)

            if bWhite:
                if numWhite % 2 == 0: # Only attach the first white LED to the resistor, since they're in series per two
                    netName = 'rWhite' + str(numRWhite)
                    # print("Looking for net: " + netName)
                    for pinNum in range(0, 3):
                        # print(pinNum)
                        connectPinToNet(dName, pinNum, netName)

                    netName = 'wToW' + str(numWhite)
                    # print("Create net: " + netName)
                    createNet(netName)
                    for pinNum_ in range(3, 6):
                        connectPinToNet(dName, pinNum_, netName)
                else: # Attach the last of the two to the W pin
                    netName = 'wToW' + str(numWhite - 1) # Net is named after the previous LED
                    # print("Looking for net: " + netName)
                    for pinNum_ in range(0, 3):
                        connectPinToNet(dName, pinNum_, netName)
                    for pinNum_ in range(3, 6):
                        connectPinToNet(dName, pinNum_, 'W')
                numWhite += 1
            else:
                if (numRGB) % 3 == 0: # first of 3
                    for index_ in range(0, 3):
                        netName = ['rGreen', 'rRed', 'rBlue'][index_ % 3] + str(int(numRGB / 3)) # rRed0, rRed1, etc
                        createNet(netName)
                        connectPinToNet(dName, index_, netName)

                        rName = 'R' + str(numRGB + (2-index_))
                        # R1, R2, R3 : D1
                        # R5, R6, R7: D7
                        # R10, R11, R12, D13
                        # R14, R15, R16, D19
                        connectPinToNet(rName, 1, netName)

                        netName = ['green', 'red', 'blue'][index_ % 3] + '0To1-' + str(int(numRGB / 3))
                        createNet(netName)
                        connectPinToNet(dName, 5 - index_, netName)
                        # R1 -> B (p[2]), R2, R (p[1]), G (p[0])
                elif (numRGB) % 3 == 1: # 2nd RGB led
                    for index_ in range(0, 3):
                        netName = ['green', 'red', 'blue'][index_ % 3] + '0To1-' + str(int(numRGB / 3))
                        connectPinToNet(dName, index_, netName)
                        netName = ['green', 'red', 'blue'][index_ % 3] + '1To2-'+ str(int(numRGB / 3))
                        createNet(netName)
                        connectPinToNet(dName, 5 - index_, netName)
                elif (numRGB) % 3 == 2: # 3rd RGB led
                    for index_ in range(0, 3):
                        netName = ['green', 'red', 'blue'][index_ % 3] + '1To2-' + str(int(numRGB / 3))
                        connectPinToNet(dName, index_, netName)
                        netName = ['G', 'R', 'B'][index_ % 3]
                        connectPinToNet(dName, 5 - index_, netName)

                numRGB += 1

            # # mod.SetValue('')

            index += 1
        i += 1

def initNets():
    names = ['12V', 'R', 'G', 'B', 'W']

    pwrClass = NETCLASSPTR('pwrClass')
    pwrClass.SetTrackWidth(FromMM(0.4))

    designSettings = board.GetDesignSettings()
    netClasses = designSettings.GetNetClasses()
    netClasses.Add(pwrClass)

    board.BuildListOfNets() # ?

    for n in names:
        net = NETINFO_ITEM(board, n, len(board.GetNetsByName()))
        pwrClass.NetNames().append(n) #
        board.Add(net)
        # if n == '12V':
        net.SetNetClass(pwrClass)

    # designSettings = board.GetDesignSettings()
    # netClasses = designSettings.m_NetClasses
    # net_classes.Add(netClass)
    # board.SetDesignSettings(design_settings)

# placeR([0,0], 'r')
initNets()
drawOutlines()
addFootPrints()
placeHolesOnTabs(tabPositions)


Refresh()
