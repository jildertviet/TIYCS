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
    index = 0
    offsets = [[0,-12.7],[12.7,0],[0,12.7],[-12.7,0]]
    for i in [[-1,0],[0,-1],[1,0],[0,1]]:
        name = 'mainConn_' + str(i)
        p = i
        p[0] *= 48
        p[1] *= 48
        p[0] += offsets[index][0]
        p[1] += offsets[index][1]
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

def placeLEDs(numPerRow=3, spacing=20, footprintName="LED_RGB_5050-6"):
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
            name = 'LED_W_' + str(index)

            mod = FootprintLoad('/Library/Application Support/kicad/modules/LED_SMD.pretty', footprintName)
            mod.SetReference(name)
            # mod.Reference().SetVisible(False)
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
            index += 1
            leds.append({"led":mod, "rotation": rotation})
    return leds

def placeResistors(leds, modulo=3, num=3):
    index = 0
    resistors = []
    for led in leds:
        offsets = [0, -2, 2]
        if index % modulo == 0:
            for j in range(num):
                footprint_lib = '/Library/Application Support/kicad/modules/Resistor_SMD.pretty'
                r = FootprintLoad(footprint_lib, 'R_0603_1608Metric')
                r.SetPosition(wxPoint(led["led"].GetPosition()[0] - FromMM(5.5), led["led"].GetPosition()[1] + FromMM(offsets[j])))
                # r.SetReference(name)
                r.Rotate(led["led"].GetPosition(), led["rotation"])
            # r.Reference().SetVisible(False)
            #
                board.Add(r)
                resistors.append(r)
        index+=1
    return resistors

initNets()
drawOutline()
placeMountingHoles()
drawHole(3.25)
placePinHeaders()
# placeWhiteLEDs()
# placeRGBLEDs()
whiteLEDs = placeLEDs(3, 20)
rgbLEDs = placeLEDs(7, 40)
whiteResistors = placeResistors(whiteLEDs, 2, 1)
rgbResistors = placeResistors(rgbLEDs, 3, 3)


Refresh()
