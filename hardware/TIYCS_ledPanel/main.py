# import numpy as np
import math
# from math import cos, sin
from pcbnew import *

board = GetBoard()

def drawEdgeLine(xStart, yStart, xEnd, yEnd):
    seg = PCB_SHAPE(board)
    board.Add(seg)
    seg.SetStart(wxPoint(FromMM(xStart), FromMM(yStart)))
    seg.SetEnd(wxPoint(FromMM(xEnd), FromMM(yEnd)))
    seg.SetLayer(Edge_Cuts)

def drawOutline():
    points = [[-1,-1],[1,-1],[1,1],[-1,1],[-1,-1]]
    for i in range(4):
        a = points[i]
        b = points[i+1]
        drawEdgeLine(a[0] * 50, a[1] * 50, b[0] * 50, b[1] * 50)

def placeMountingHoles():
    points = [[-1,-1],[1,-1],[1,1],[-1,1]]
    i = 0
    scale = 46
    for p in points:
        mod = FootprintLoad('/Library/Application Support/kicad/modules/MountingHole.pretty', 'MountingHole_3mm_Pad')
        mod.SetPosition(wxPoint(FromMM(p[0] * scale), FromMM(p[1] * scale)))
        mod.SetReference('H_' + str(i))
        mod.Reference().SetVisible(False)
        board.Add(mod)
        i += 1

def drawHole(r):
    seg = PCB_SHAPE(board)
    board.Add(seg)
    seg.SetShape(S_CIRCLE)
    seg.SetCenter(wxPoint(0,0))
    # seg.SetArcStart(wxPoint(FromMM(r), 0))
    seg.SetArcGeometry(wxPoint(0,0) + wxPoint(FromMM(r), 0), wxPoint(0,0), wxPoint(0,0))
    seg.SetLayer(Edge_Cuts)

def initNets():
    names = ['12V', 'R', 'G', 'B', 'W']
    for n in names:
        net = NETINFO_ITEM(board, n, len(board.GetNetsByName()))
        board.Add(net)

def connectPinToNet(componentName, pinIndex, netName):
    footprint = board.FindFootprintByReference(componentName)
    vin = board.GetNetsByName().find(netName).value()[1]
    footprint.Pads()[pinIndex].SetNet(vin)

def placePinHeaders():
    for h in range(2):
        index = 0
        offsets = [[0,-12.7],[12.7,0],[0,12.7],[-12.7,0]]
        offsets2 = [[2.54,0],[0,2.54],[-2.54,0],[0,-2.54]]
        for i in [[-1,0],[0,-1],[1,0],[0,1]]:
            name = 'mainConn_' + str(index) + '_' + str(h)
            p = i
            p[0] *= 48;
            p[1] *= 48;
            p[0] += offsets[index][0] + (offsets2[index][0] * h)
            p[1] += offsets[index][1] + (offsets2[index][1] * h)
            mod = FootprintLoad('/Library/Application Support/kicad/modules/Connector_PinHeader_2.54mm.pretty', 'PinHeader_1x11_P2.54mm_Vertical')
            mod.SetPosition(wxPoint(FromMM(p[0]), FromMM(p[1])))
            mod.Rotate(mod.GetPosition(), 900 * -index)
            mod.SetReference(name)
            mod.Reference().SetVisible(False)
            board.Add(mod)
            # Connect pins: 12V, W, B, G, R, 12V, R, G, B, W, 12V
            connectPinToNet(name, 5, '12V')
            j = 0
            for netName in ['12V','W','B','G','R']:
                connectPinToNet(name, 10-j, netName)
                connectPinToNet(name, j, netName)
                j+=1
            index += 1

def placeLEDs(numPerRow=3, spacing=20, color='W', footprintName="LED_RGB_5050-6"):
    leds = []
    index = 0
    for i in range(4):
        p = [[-1,-1],[1,-1],[1,1],[-1,1]][i]
        p[0] *= spacing
        p[1] *= spacing
        dir = [[1,0],[0,1],[-1,0],[0,-1]][i]
        p[0] += dir[0] * 1 # Offsets
        p[1] += dir[1] * 1
        for j in range(numPerRow):
            name = 'LED_' + color + '_' + str(index)

            mod = FootprintLoad('/Library/Application Support/kicad/modules/LED_SMD.pretty', footprintName)
            mod.SetReference(name)
            mod.Reference().SetVisible(False)
            mod.SetPosition(wxPoint(FromMM(p[0]), FromMM(p[1])))

            # if j == 0:
                # mod.Rotate(mod.GetPosition(), 450 + (900*-i))
                # mod.SetPosition(wxPoint(mod.GetPosition()[0]*0.85, mod.GetPosition()[1]*0.85))
            # else:
            rotation = 900*-i
            mod.Rotate(mod.GetPosition(), rotation)

            board.Add(mod)

            p[0] += dir[0] * ((spacing*2) / numPerRow)
            p[1] += dir[1] * ((spacing*2) / numPerRow)
            leds.append({"led":mod, "rotation": rotation, "id":index})
            index += 1
    return leds

def placeResistors(leds, modulo=3, num=3, color='W'):
    index = 0
    resistors = []
    for led in leds:
        offsets = [0, -2, 2]
        if index % modulo == 0:
            for j in range(num):
                footprint_lib = '/Library/Application Support/kicad/modules/Resistor_SMD.pretty'
                r = FootprintLoad(footprint_lib, 'R_0603_1608Metric')
                r.SetPosition(wxPoint(led["led"].GetPosition()[0] - FromMM(6), led["led"].GetPosition()[1] + FromMM(offsets[j])))
                r.SetReference('R_' + str(led["id"]) + '_' + str(j) + '_' + color) #R_0_0_W
                r.Rotate(led["led"].GetPosition(), led["rotation"])
                r.Reference().SetVisible(False)

                board.Add(r)
                resistors.append(r)
        index+=1
    return resistors

def createNet(netName):
    net = NETINFO_ITEM(board, netName, len(board.GetNetsByName()))
    board.Add(net)

def placeRGBNets(leds, resistors):
    index = 0
    for led in leds:
        if index % 3 == 0:
            #Connect Resistor both pads
            connectPinToNet(resistors[index].GetReference(), 0, '12V')
            connectPinToNet(resistors[index+1].GetReference(), 0, '12V')
            connectPinToNet(resistors[index+2].GetReference(), 0, '12V')

            indexTemp = 0
            for color in ['Red','Green','Blue']: # This is already swapped when reading the leds (ordered: center, top, bottom)
                netName = 'RtoD' + str(led["id"]) + '_' + color
                createNet(netName)
                connectPinToNet(resistors[index+indexTemp].GetReference(), 1, netName)
                #Connect Led0 pads 1-3
                connectPinToNet(leds[index]["led"].GetReference(), [1, 0, 2][indexTemp], netName) # Map to pins 1, 0, 2 instead of 0, 1, 2
                # Connect Led0 to Led1
                netName = 'D' + str(leds[index]["id"]) + 'toD' + str(leds[index+1]["id"]) + '_' + color
                createNet(netName)
                connectPinToNet(leds[index]["led"].GetReference(), [4,5,3][indexTemp], netName)
                connectPinToNet(leds[index+1]["led"].GetReference(), [1,0,2][indexTemp], netName)
                # Connect Led1 to Led2
                netName = 'D' + str(leds[index+1]["id"]) + 'toD' + str(leds[index+2]["id"]) + '_' + color
                createNet(netName)
                connectPinToNet(leds[index+1]["led"].GetReference(), [4,5,3][indexTemp], netName)
                connectPinToNet(leds[index+2]["led"].GetReference(), [1,0,2][indexTemp], netName)
                # Connect Led2 to pins
                connectPinToNet(leds[index+2]["led"].GetReference(), [5,4,3][indexTemp], ['G','R','B'][indexTemp])
                indexTemp+=1
        index+=1

def placeWhiteNets(leds, resistors):
    index = 0
    for led in leds:
        if index % 2 == 0:
            #Connect Resistor both pads
            connectPinToNet(resistors[int(index/2)].GetReference(), 0, '12V')

            netName = 'RtoD' + str(led["id"]) + '_W'
            createNet(netName)
            connectPinToNet(resistors[int(index/2)].GetReference(), 1, netName)
            indexTemp = 0
            for i in range(3):
                #Connect Led0 pads 1-3
                connectPinToNet(leds[index]["led"].GetReference(), [0, 1, 2][i], netName) # Map to pins 1, 0, 2 instead of 0, 1, 2
                # Connect Led0 to Led1
            for i in range(3):
                netName = 'D' + str(leds[index]["id"]) + 'toD' + str(leds[index+1]["id"]) + '_W'
                createNet(netName)
                connectPinToNet(leds[index]["led"].GetReference(), [4,5,3][i], netName)
                connectPinToNet(leds[index+1]["led"].GetReference(), [1,0,2][i], netName)
            for i in range(3):
                # # Connect Led1 to W net
                connectPinToNet(leds[index+1]["led"].GetReference(), [5,4,3][i], 'W')
        index+=1

initNets()
drawOutline()
placeMountingHoles()
drawHole(3.25)
placePinHeaders()
# placeWhiteLEDs()
# placeRGBLEDs()
whiteLEDs = placeLEDs(3, 20, 'W')
rgbLEDs = placeLEDs(6, 40, 'RGB')
whiteResistors = placeResistors(whiteLEDs, 2, 1, 'W')
rgbResistors = placeResistors(rgbLEDs, 3, 3, 'RGB')
placeRGBNets(rgbLEDs, rgbResistors)
placeWhiteNets(whiteLEDs, whiteResistors)

rgbLEDs = placeLEDs(3, 30, 'RGB2')
rgbResistors = placeResistors(rgbLEDs, 3, 3, 'RGB')
placeRGBNets(rgbLEDs, rgbResistors)
whiteLEDs = placeLEDs(1, 7.5, 'W2')
whiteResistors = placeResistors(whiteLEDs, 2, 1, 'W2')
placeWhiteNets(whiteLEDs, whiteResistors)

Refresh()
