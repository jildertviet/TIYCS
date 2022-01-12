n = NetAddr("127.0.0.1", 5000);
n.sendMsg("/setId", 0);



n.sendMsg("/setMode", 9); // Kapitein
(
var num = 1;
n.sendMsg("/setMode", 13); // Bingo mode
// Pick a bingo number

// n.sendMsg("/setMode", 7);
~setVal.value(0, num);
n.sendMsg("/eventById", 2, num);
)

~setVal = {|id, value| n.sendMsg("/setValueById", id, value)};
~setVal.value(3, 4);

n.sendMsg("/setMode", 3);
n.sendMsg("/eventById", 0)

n.sendMsg("setValueById", 2, 0.0); // All black
~setVal.value(2, 255);

// Land / steen
n.sendMsg("/setMode", 2);

// Tank
n.sendMsg("/setMode", 1);
		n.sendMsg("/setValueById", 1, 0);
// Tank-vollopen
(
{
	4.wait;
	150.do{
		|i|
		n.sendMsg("/setValueById", 1, i);
		((1/150) * 5).wait;
	}
}.fork;
)

// Play first commercial
(
var movie = 0;
n.sendMsg("/setMode", 3 + movie);
n.sendMsg("/eventById", 1, movie) // Play video 0
)

57*8

x = (1/150)*2

x * 150


n.sendMsg("/setMode", 10);

n.sendMsg("/setMode", 11); // Instrunctions
~setVal.value(3, 1);

~showFlightInfo = {~setVal.value(4, true)};
~showFlightInfo.value
~updateFlightInfo = {|h,s,t| ~setVal.value(5, h, s, t)};


~setVal.value(6, 3); // Code 1


~setVal.value(7, 25);