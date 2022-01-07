//             dName = 'D_' + str(len(rings)) + '_' + str(indexInThisRing)
    for n in [[25, 45, 0.014], [20, 36, 0.017], [14, 27, 0.025], [10, 15, 0.1]]: # Num, r
(
var rgb = "";
var white = "";
var numRGB = 0;
var numWhite = 0;
var resistors = List.new()!4;
	[25, 20, 10, 10].do{
		|num, ringID|
		num.do{
			|index|
			var name = "D_" ++ (ringID+1) ++ "_" ++ index;
		if(index % 5 == 0, {
			var rgbLedName = "R" ++ (ringID+1) ++ "_" ++ index;
			3.do{
				|i|
				var rgbLedNameTotal = rgbLedName ++ "_" ++ i;
				// rgbLedNameTotal.postln;
				resistors[i].add(	rgbLedNameTotal);
			};
		});
		if(index % 5 == 2, {
			var rgbLedName = "R" ++ (ringID+1) ++ "_" ++ index;
			resistors[3].add(rgbLedName);
		});
		if(index % 5 < 3, {
			rgb = rgb ++ "," ++ name;
			numRGB = numRGB + 1;
		}, {
			white = white ++ "," ++ name;
			numWhite = numWhite + 1;
		});
		// name.postln;
		};
	};
// rgb.postln;
// white.postln;
// numRGB.postln;
// numWhite.postln;
resistors[3].postln;
// resistors[1].postln;
// resistors[2].postln;
)

// R[2] == pin[0]
// R[0] == pin[2]
// R[1] == pin[1]

(65 / 5)


2 + 3 + 2 + 3 + 2 + 3 + 2 + 3 + 2+ 3 + 2 + 3



"D1, D3, D5, D7, D9, D11, D13, D15, D17, D19, D21, D23, D25, D27, D29".split($,).size



"RW4,R8,R11,R9,R10,RW5,RW7,R2,R7,R6,RW0,R4,R5,RW6,R12,R13,R14,R0,R1,RW1,R3,RW2,RW3".split($,).do{|e|e.postln}
5.do{|i| ("R" ++ (i*3+2)).postln}