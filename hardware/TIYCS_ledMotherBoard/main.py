# https://www.omnicalculator.com/math/regular-polygon
# Import math Library
import numpy as np
import math
from math import cos, sin
from pcbnew import *
# import pcbnew
board = GetBoard()
io = PCB_IO()

boardSize = [44.142, 60.00] # In mm

def rotation_matrix(axis, theta):
    axis = np.asarray(axis)
    axis = axis/math.sqrt(np.dot(axis, axis))
    a = math.cos(theta/2.0)
    b, c, d = -axis*math.sin(theta/2.0)
    aa, bb, cc, dd = a*a, b*b, c*c, d*d
    bc, ad, ac, ab, bd, cd = b*c, a*d, a*c, a*b, b*d, c*d
    return np.array([[aa+bb-cc-dd, 2*(bc+ad), 2*(bd-ac)],
                     [2*(bc-ad), aa+cc-bb-dd, 2*(cd+ab)],
                     [2*(bd+ac), 2*(cd-ab), aa+dd-bb-cc]])

# For layer IDs: Definition at line 65 of file layer_ids.h.
def drawEdgeLine(xStart, yStart, xEnd, yEnd):
    return
    seg = PCB_SHAPE(board)
    board.Add(seg)
    seg.SetStart(wxPoint(FromMM(xStart), FromMM(yStart)))
    seg.SetEnd(wxPoint(FromMM(xEnd), FromMM(yEnd)))
    seg.SetLayer(Edge_Cuts)

tabPositions = []
rInner = 53.28
R = 57.6744 # Outter circle radius, based on side len: 44.142mm
axis = [0, 0, 1]

def rotateAroundZAxis(vec, angle):
    rotated = np.dot(rotation_matrix(axis,angle), vec)
    return [float(rotated[0]), float(rotated[1]), float(rotated[2])]

def drawOutlines():
    print('x')
    points = []
    for i in range(10):
        ph = (i * (1./8.) + (1./16.)) * (math.pi * 2)
        points.append([math.cos(ph) * R, math.sin(ph) * R])
    for i in range(len(points)-2): # Only treat 8 sides
        ph = i * (1./8.) * (math.pi * 2)
        numTabs = 9
        tabWidth = 1.6
        tabSpacing = 4
        for tabIndex in range(numTabs):
            index = tabIndex - int(numTabs / 2) # -2 -1 0 1 2
            center = 0
            xStart = center - (tabWidth * 0.5)
            xEnd = center + (tabWidth * 0.5)
            yStart = 0 - tabWidth + rInner
            yEnd = 0 + rInner
            xStart -= (tabSpacing * index) # Offset by index, so they get spaced on X-axis
            xEnd -= (tabSpacing * index)

            startPos = [xStart, yStart, 0]
            endPos = [xEnd, yStart, 0]
            rotatedStartPos = rotateAroundZAxis(startPos, ph)
            rotatedEndPos = rotateAroundZAxis(endPos, ph)

            tabCenter = rotateAroundZAxis([xStart + tabWidth*0.5, yStart, 0], ph)
            tabPositions.append([tabCenter[0], tabCenter[1], ph])

            # print(startPos)
            # print(rotatedStartPos[0])
            # print(rotatedStartPos[1])
            drawEdgeLine(rotatedStartPos[0], rotatedStartPos[1], rotatedEndPos[0], rotatedEndPos[1])
            endPos = [xStart, yEnd, 0]
            rotatedEndPos = rotateAroundZAxis(endPos, ph)
            drawEdgeLine(rotatedStartPos[0], rotatedStartPos[1], rotatedEndPos[0], rotatedEndPos[1])
            endPos = [xEnd, yEnd, 0]
            rotatedEndPos = rotateAroundZAxis(endPos, ph)
            startPos = [xEnd, yStart, 0]
            rotatedStartPos = rotateAroundZAxis(startPos, ph)
            drawEdgeLine(rotatedStartPos[0], rotatedStartPos[1], rotatedEndPos[0], rotatedEndPos[1])

            if tabIndex != numTabs -1: # Connect the tabs
                nextX = center + (tabWidth * 0.5) - (tabSpacing * (index + 1))
                startPos = [xStart, yEnd, 0]
                endPos = [nextX, yEnd, 0]
                rotatedStartPos = rotateAroundZAxis(startPos, ph)
                rotatedEndPos = rotateAroundZAxis(endPos, ph)
                drawEdgeLine(rotatedStartPos[0], rotatedStartPos[1], rotatedEndPos[0], rotatedEndPos[1])
                # drawEdgeLine(xStart, yEnd, nextX, yEnd)
            if tabIndex == 0: # Connect left tab to left corner of board
                endPos = [xEnd, yEnd, 0]
                rotatedEndPos = rotateAroundZAxis(endPos, ph)
                drawEdgeLine(rotatedEndPos[0], rotatedEndPos[1], points[i+1][0], points[i+1][1])
                # drawEdgeLine(xEnd, yEnd, boardSize[0], yEnd)
            if tabIndex == numTabs -1: # Right to right
                endPos = [xStart, yEnd, 0]
                rotatedEndPos = rotateAroundZAxis(endPos, ph)
                drawEdgeLine(rotatedEndPos[0], rotatedEndPos[1], points[i+2][0], points[i+2][1])
                # drawEdgeLine(0, yEnd, xStart, yEnd)
def drawHole(r):
    seg = PCB_SHAPE(board)
    board.Add(seg)
    seg.SetShape(S_CIRCLE)
    seg.SetCenter(wxPoint(0,0))
    seg.SetArcStart(wxPoint(FromMM(r), 0))
    seg.SetLayer(Edge_Cuts)

def placeHolesOnTabs(positions):
    numHoles = 0
    footprint_lib = '/Users/jildertviet/Documents/kiCadLibs/jildertTest.pretty'
    for p in positions:
        mod = FootprintLoad(footprint_lib, 'jCastellatedHole')
        mod.SetPosition(wxPoint(FromMM(p[0]), FromMM(p[1] - 0.15)))
        mod.Rotate(mod.GetPosition(), (math.degrees(p[2]) + 180) * 10 * -1)
        # mod.SetValue('')
        pName = 'p' + str(numHoles)
        mod.SetReference(pName)
        mod.Reference().SetVisible(False)
        board.Add(mod)
        net = ['12V', '', 'R', 'G', '12V', 'B', 'W', '', '12V'][numHoles % 9]
        if net != '':
            connectPinToNet(pName, 0, net)

        numHoles += 1

numR = 0
def placeR(pos, name, rotation):
    footprint_lib = '/Library/Application Support/kicad/modules/Resistor_SMD.pretty'
    r = FootprintLoad(footprint_lib, 'R_0603_1608Metric')
    r.SetPosition(wxPoint(FromMM(pos[0]), FromMM(pos[1])))
    # mod.SetValue('')
    r.SetReference(name)
    r.Rotate(r.GetPosition(), rotation)
    r.Reference().SetVisible(False)

    board.Add(r)
    global numR
    numR += 1

def connectPinToNet(componentName, pinIndex, netName):
    r = board.FindFootprintByReference(componentName)
    # print(r)
    vin = board.GetNetsByName().find(netName).value()[1]
    # print(vin)
    r.Pads()[pinIndex].SetNet(vin)

def createNet(netName):
    net = NETINFO_ITEM(board, netName, len(board.GetNetsByName()))
    board.Add(net)

rgbLeds = []
def addFootPrints():
    colPerRow = [3, 3, 2, 1, 1]
    ledSpacingHor = [10, 9, 10, 10, 10]
    offsetHor = [0, 0, 0, 0, 0]
    ledSpacingVer = 8.5
    numLed = 0
    numWhite = 0
    numRGB = 0
    yOffset = 8
    numR = 0
    previousWhiteLED = 'D0'

    for i in range(8):
        ph = i * (1./8.) * (math.pi * 2)
        rowIndex = 0
        for y in colPerRow:
            if i % 2 == 1 and rowIndex == 2:
                y = 0
            if i % 2 == 1 and rowIndex == 4:
                y = 0
            # print("Part # " + str(i) + ", place " + str(y) + " on this row")
            for x in range(y):
                if(y%2==1):
                    ledIndex = x - math.floor(y*0.5)
                else:
                    ledIndex = (x+0.5) - (y*0.5)
                if numLed == 42:
                    numLed += 1
                    numR += 1
                    continue
                yPos = -rInner + (rowIndex * ledSpacingVer) + yOffset
                xPos = ledSpacingHor[rowIndex] * ledIndex + offsetHor[rowIndex]
                rotatedPos = rotateAroundZAxis([xPos, yPos, 0], ph)
                dName = 'D' + str(numLed)
                footprintName = 'LED_RGB_5050-6' # RGB
                mod = FootprintLoad('/Library/Application Support/kicad/modules/LED_SMD.pretty', footprintName)
                mod.SetReference(dName)
                mod.Reference().SetVisible(False)
                mod.SetPosition(wxPoint(FromMM(rotatedPos[0]), FromMM(rotatedPos[1])))
                rotation = math.degrees(ph) * 10 * -1
                mod.Rotate(mod.GetPosition(), rotation)
                board.Add(mod)

                rName = 'R' + str(numR)
                rotatedPos = rotateAroundZAxis([xPos, yPos + 4, 0], ph)

                if numLed % 2 == 0: # White
                    # mod.Reference().SetVisible(True)
                    if numWhite % 2 == 0:
                        previousWhiteLED = dName
                        placeR([rotatedPos[0], rotatedPos[1]], rName, rotation)
                        connectPinToNet(rName, 0, '12V')
                        netName = rName + 'to' + dName
                        createNet(netName)
                        connectPinToNet(rName, 1, netName)
                        connectPinToNet(dName, 0, netName)
                        connectPinToNet(dName, 1, netName)
                        connectPinToNet(dName, 2, netName)
                    else:
                        a = 0
                        prevLed = previousWhiteLED
                        netName = prevLed + 'to' + dName
                        createNet(netName)
                        connectPinToNet(dName, 3, 'W')
                        connectPinToNet(dName, 4, 'W')
                        connectPinToNet(dName, 5, 'W')
                        connectPinToNet(dName, 0, netName)
                        connectPinToNet(dName, 1, netName)
                        connectPinToNet(dName, 2, netName)
                        connectPinToNet(prevLed, 3, netName)
                        connectPinToNet(prevLed, 4, netName)
                        connectPinToNet(prevLed, 5, netName)
                        # Connect pins 1 2 3 to 4 5 6 or previous whiteLED
                    numWhite += 1
                else: # RGB
                    rName = 'rgbR' + str(numLed)
                    # print("Place" + rName)
                    placeR([rotatedPos[0], rotatedPos[1]], rName, rotation)
                    connectPinToNet(rName, 0, '12V')
                    rgbLeds.append(numLed)
                numLed += 1
                numR += 1
            rowIndex += 1

def connectRGB():
    for i in range(0, int(len(rgbLeds)/3)): #D1, D3, D5, D7
        index = i * 3
        # print("RGB series starts @ D" + str(rgbLeds[index]))
        id = rgbLeds[index]

        # Connect the resistors to the first LED
        tNetNames = ['R' + str(id) + 'toD' + str(id) + '_Green', 'R' + str(id) + 'toD' + str(id) + '_Red', 'R' + str(id) + 'toD' + str(id) + '_Blue'] # R1toD1_Red, R2ToD1_Green, R3ToD1_Blue
        for n in tNetNames:
            createNet(n)
        # print("Find: " + 'rgbR' + str(id))
        connectPinToNet('rgbR' + str(id), 1, tNetNames[0])
        connectPinToNet('D' + str(id), 0, tNetNames[0])
        connectPinToNet('rgbR' + str(id + 2), 1, tNetNames[1])
        connectPinToNet('D' + str(id), 1, tNetNames[1])
        connectPinToNet('rgbR' + str(id + 4), 1, tNetNames[2])
        connectPinToNet('D' + str(id), 2, tNetNames[2])
        # Connect 1st LED to 2nd
        tNetNames = []
        for x in ['Green', 'Red', 'Blue']:
            tNetNames.append('D' + str(id) + 'toD' + str(id+2) + '_' + x)
        for n in tNetNames:
            createNet(n)
        connectPinToNet('D' + str(id), 5, tNetNames[0])
        connectPinToNet('D' + str(id+2), 0, tNetNames[0])
        connectPinToNet('D' + str(id), 4, tNetNames[1])
        connectPinToNet('D' + str(id+2), 1, tNetNames[1])
        connectPinToNet('D' + str(id), 3, tNetNames[2])
        connectPinToNet('D' + str(id+2), 2, tNetNames[2])
        #connect 2nd led to 3rd led
        tNetNames = []
        for x in ['Green', 'Red', 'Blue']:
            tNetNames.append('D' + str(id+2) + 'toD' + str(id+4) + '_' + x)
        for n in tNetNames:
            createNet(n)
        connectPinToNet('D' + str(id+2), 5, tNetNames[0])
        connectPinToNet('D' + str(id+4), 0, tNetNames[0])
        connectPinToNet('D' + str(id+2), 4, tNetNames[1])
        connectPinToNet('D' + str(id+4), 1, tNetNames[1])
        connectPinToNet('D' + str(id+2), 3, tNetNames[2])
        connectPinToNet('D' + str(id+4), 2, tNetNames[2])
        #Connect 3rd led to 'R', 'G', and 'B'
        connectPinToNet('D' + str(id+4), 3, 'B')
        connectPinToNet('D' + str(id+4), 4, 'R')
        connectPinToNet('D' + str(id+4), 5, 'G')

def initNets():
    names = ['12V', 'R', 'G', 'B', 'W']
    for n in names:
        net = NETINFO_ITEM(board, n, len(board.GetNetsByName()))
        board.Add(net)

def placeMountingHoles():
    # 40 mm betwoon mounting holes, so
    points = [[-1,-1],[1,-1],[1,1],[-1,1]]
    i = 0
    for p in points:
        mod = FootprintLoad('/Library/Application Support/kicad/modules/MountingHole.pretty', 'MountingHole_3mm_Pad')
        mod.SetPosition(wxPoint(FromMM(p[0] * 20), FromMM(p[1] * 20)))
        mod.SetReference('H_' + str(i))
        mod.Reference().SetVisible(False)
        board.Add(mod)
        i += 1

def placePinHeader():
    mod = FootprintLoad('/Library/Application Support/kicad/modules/Connector_PinHeader_2.54mm.pretty', 'PinHeader_1x08_P2.54mm_Vertical')
    mod.SetPosition(wxPoint(FromMM(-8.875), FromMM(20)))
    mod.Rotate(wxPoint(FromMM(-8.875), FromMM(20)), 900)
    mod.SetReference('mainConn')
    mod.Reference().SetVisible(False)
    board.Add(mod)
    for i in range(4):
        connectPinToNet('mainConn', i, '12V')
    connectPinToNet('mainConn', 4, 'G')
    connectPinToNet('mainConn', 5, 'R')
    connectPinToNet('mainConn', 6, 'B')
    connectPinToNet('mainConn', 7, 'W')

rings = []
rgbLedsAsSeries = [[[],[]]]
whiteLedsAsSeries = [[]]
def addFootPrintsInCircle():
    ledIndex = 0
    for n in [[25, 45, 0.014], [20, 36, 0.017], [14, 27, 0.025], [10, 15, 0.1]]: # Num, r
        rings.append([])
        ph = 0
        r = n[1]
        phAdd = (1/n[0])
        indexInThisRing = 0
        for i in range(n[0]):
            x = math.cos(ph * (math.pi*2)) * r
            y = math.sin(ph * (math.pi*2)) * r

            # phAdd -= 0.1
            # r -= 0.1

            # rotatedPos = rotateAroundZAxis([x, y, 0], ph)
            dName = 'D_' + str(len(rings)) + '_' + str(indexInThisRing)
            if ledIndex not in [47, 50, 54, 57]:
                footprintName = 'LED_RGB_5050-6' # RGB
                mod = FootprintLoad('/Library/Application Support/kicad/modules/LED_SMD.pretty', footprintName)
                mod.SetReference(dName)
                # mod.Reference().SetVisible(False)
                mod.SetPosition(wxPoint(FromMM(x), FromMM(y)))
                rotation = (math.degrees(ph * (math.pi*2)) + 90) * 10 * -1
                mod.Rotate(mod.GetPosition(), rotation)
                board.Add(mod)

                if indexInThisRing % 5 < 3: # RGB leds
                    rgbLedsAsSeries[-1][0].append(mod) # [[[D0],[]]]
                else:
                    whiteLedsAsSeries[-1][0].append(mod) # [[[D0],[]]]

                if indexInThisRing % 5 == 4:
                    rgbLedsAsSeries.append([[],[]]) # New array for new series
                if indexInThisRing % 5 == 0:
                    whiteLedsAsSeries.append([[],[]]) # New array for new series

                if indexInThisRing % 5 == 0:
                    numRTemp = 0
                    if len(rings) < 4:
                        for numVert in [0, 1, -1]:
                            x = math.cos((ph + (n[2] * numVert)) * (math.pi*2)) * (r - 4)
                            y = math.sin((ph + (n[2] * numVert)) * (math.pi*2)) * (r - 4)
                            footprint_lib = '/Library/Application Support/kicad/modules/Resistor_SMD.pretty'
                            resistor = FootprintLoad(footprint_lib, 'R_0603_1608Metric')
                            resistor.SetPosition(wxPoint(FromMM(x), FromMM(y)))
                            # mod.SetValue('')
                            rName = 'R' + str(len(rings)) + '_' + str(indexInThisRing) + '_' + str(numRTemp)
                            resistor.SetReference(rName)
                            rotation = (math.degrees((ph + (n[2] * numVert)) * (math.pi*2)) + 90) * 10 * -1
                            resistor.Rotate(resistor.GetPosition(), rotation)
                            board.Add(resistor)

                            rgbLedsAsSeries[-1][1].append(resistor) # [[[D0],[]]]
                            numRTemp += 1
                    else:
                        for numVert in range(3):
                            x = math.cos((ph) * (math.pi*2)) * (r - 4 - (numVert*1.7))
                            y = math.sin((ph) * (math.pi*2)) * (r - 4 - (numVert*1.7))
                            footprint_lib = '/Library/Application Support/kicad/modules/Resistor_SMD.pretty'
                            resistor = FootprintLoad(footprint_lib, 'R_0603_1608Metric')
                            resistor.SetPosition(wxPoint(FromMM(x), FromMM(y)))
                            # mod.SetValue('')
                            rName = 'R' + str(len(rings)) + '_' + str(indexInThisRing) + '_' + str(numRTemp)
                            resistor.SetReference(rName)
                            rotation = (math.degrees((ph) * (math.pi*2)) + 90) * 10 * -1
                            resistor.Rotate(resistor.GetPosition(), rotation)
                            board.Add(resistor)
                            rgbLedsAsSeries[-1][1].append(resistor)
                            numRTemp += 1

                elif indexInThisRing % 5 == 3: # WHITE LEDS
                    if len(rings) == 3 and indexInThisRing == 3:
                        offset = 0.02
                        rotation = (math.degrees((ph - offset) * (math.pi*2)) + 90) * 10 * -1
                        x = math.cos((ph - offset) * (math.pi*2)) * (r - 4)
                        y = math.sin((ph - offset) * (math.pi*2)) * (r - 4)
                    else:
                        rotation = (math.degrees((ph) * (math.pi*2)) + 90) * 10 * -1
                        x = math.cos((ph) * (math.pi*2)) * (r - 4)
                        y = math.sin((ph) * (math.pi*2)) * (r - 4)
                    footprint_lib = '/Library/Application Support/kicad/modules/Resistor_SMD.pretty'
                    resistor = FootprintLoad(footprint_lib, 'R_0603_1608Metric')
                    resistor.SetPosition(wxPoint(FromMM(x), FromMM(y)))
                    # mod.SetValue('')
                    rName = 'R' + str(len(rings)) + '_' + str(indexInThisRing)
                    resistor.SetReference(rName)

                    resistor.Rotate(resistor.GetPosition(), rotation)
                    board.Add(resistor)
                    whiteLedsAsSeries[-1][1].append(resistor) # [[[D0],[]]]

                ledIndex += 1
                rings[-1].append(mod)
                indexInThisRing += 1
            else:
                ledIndex += 1


            ph += phAdd

            # r -= 0.5
            # phAdd -= 0.05
def connectFootPrints():
    for r in rings:
        # print(len(r))
        ledIndex = 0
        rgbIndex = 0
        whiteIndex = 0
        for led in r:
            if ledIndex % 5 < 3:
                print("RGB")
                if rgbIndex == 0:
                    print("first")
                    offset = [-2, -5.25]
                    # placeR([ToMM(led.GetPosition()[0]) + offset[0], ToMM(led.GetPosition()[1]) + offset[1]], 'R', led.GetOrientationDegrees() * 10.);
                    footprint_lib = '/Library/Application Support/kicad/modules/Resistor_SMD.pretty'
                    r = FootprintLoad(footprint_lib, 'R_0603_1608Metric')
                    r.SetPosition(led.GetPosition())
                    # mod.SetValue('')
                    r.SetReference('r')
                    r.Rotate(r.GetPosition(), led.GetOrientationDegrees() * 10.)

                    dir = np.array(led.GetPosition())
                    dir = dir/np.linalg.norm(dir)

                    theta = np.deg2rad(1)
                    rot = np.array([[cos(theta), -sin(theta)], [sin(theta), cos(theta)]])
                    dir = np.dot(rot, dir)

                    # print(dir)
                    radius = np.array([ToMM(led.GetPosition()[0]), ToMM(led.GetPosition()[1])])
                    radius = np.linalg.norm(np.array([0,0]) - radius)
                    # radius -= 1.5
                    x = float(dir[(0)])
                    y = float(dir[(1)])

                    r.SetPosition(wxPoint(FromMM(x) * radius, FromMM(y) * radius))

                    r.Reference().SetVisible(False)

                    board.Add(r)
                    # Place 3 R
                    # connect pin 4, 5 and 6 to net with next LED
                elif rgbIndex == 1:
                    print("second")
                    # 1, 2, 3 to previous defined net
                    # 4, 5 and 6 to new define net
                elif rgbIndex == 2:
                    print("third")
                    # 1, 2, 3 to previous defined net
                    # 4, 5 and 6 to G R and B power lines
                    rgbIndex = -1 # RESET
                rgbIndex +=1
            else:
                print("White")
                if whiteIndex == 0:
                    print("first")
                elif whiteIndex == 1:
                    print("second")
                    whiteIndex = -1 # RESET
                whiteIndex +=1
            ledIndex +=1

def addNets():
    for serie in rgbLedsAsSeries:
        for resistor in serie[1]:
            connectPinToNet(resistor.GetReference(), 1, "12V")

        leds = serie[0]
        if len(leds) == 0:
            continue
        resistors = serie[1]
        # Connect resistors to first LED
        netName = leds[0].GetReference() + "_" + resistors[0].GetReference()
        createNet(netName)
        connectPinToNet(resistors[0].GetReference(), 0, netName)
        connectPinToNet(leds[0].GetReference(), 2, netName)

        netName = leds[0].GetReference() + "_" + resistors[1].GetReference()
        createNet(netName)
        connectPinToNet(resistors[1].GetReference(), 0, netName)
        connectPinToNet(leds[0].GetReference(), 1, netName)

        netName = leds[0].GetReference() + "_" + resistors[2].GetReference()
        createNet(netName)
        connectPinToNet(resistors[2].GetReference(), 0, netName)
        connectPinToNet(leds[0].GetReference(), 0, netName)

        grb = ['G', 'R', 'B']
        for i in range(3):
            for j in range(2):
                netName = leds[j].GetReference() + "_" + leds[j+1].GetReference() + "_" + grb[i] # D0_D1_R
                createNet(netName)
                connectPinToNet(leds[j].GetReference(), 5 - i, netName)
                connectPinToNet(leds[j+1].GetReference(), 0 + i, netName)
            connectPinToNet(leds[2].GetReference(), 3+i, ['B','R','G'][i])

    for serie in whiteLedsAsSeries:
        if len(serie) == 0:
            continue
        resistor = serie[1][0]
        leds = serie[0]
        if len(leds) == 0:
            continue
        netName = resistor.GetReference() + "_" + leds[0].GetReference()
        createNet(netName)
        ledToLedNetName = leds[0].GetReference() + "_" + leds[1].GetReference()
        createNet(ledToLedNetName)

        connectPinToNet(resistor.GetReference(), 0, netName)
        connectPinToNet(resistor.GetReference(), 1, '12V')
        for i in range(3):
            connectPinToNet(leds[0].GetReference(), i, netName)
            connectPinToNet(leds[0].GetReference(), 3+i, ledToLedNetName)
            connectPinToNet(leds[1].GetReference(), i, ledToLedNetName)
            connectPinToNet(leds[1].GetReference(), 3+i, 'W')

# initNets()
# drawOutlines()
# drawHole(3)
# addFootPrintsInCircle()
# addNets()

# placeHolesOnTabs(tabPositions)

# placeMountingHoles()
# placePinHeader()

Refresh()
