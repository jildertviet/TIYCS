(
~screens = [
	// NetAddr("192.168.0.2", 5000),
	NetAddr("127.0.0.1", 5000),
	NetAddr("127.0.0.1", 5001),
	// NetAddr("192.168.0.4", 5000),
	// NetAddr("192.168.0.3", 5000),
];
~setVal = {
	|id, value,screen=0|
	if(screen == -1, {
		~screens.do{|g| g.sendMsg("/setValueById", id, value)};
	}, {
		~screens[screen].sendMsg("/setValueById", id, value);
	});
};
~setMode = {
	|id, screen=0|
	if(screen == -1, {
		~screens.do{|g| g.sendMsg("/setMode", id)};
	}, {
		~screens[screen].sendMsg("/setMode", id)
	});
};
~screenBrightness = {
	|val|
	~setVal.value(2, val * 255, -1);
	~screenBrSlider.value = val;
};
)


~setVal.value(4, true); // Flight info
~setVal.value(4, false); // Flight info off

~setMode.value(13,0);
~rolling.stop; ~rolling = {inf.do{~setVal.value(9, [0,1, 0.5].choose, 0); 0.5.wait}}.fork; // Start rolling
~rolling.stop; ~setVal.value(9, 1, 0); // Stop
// 4 5 6 9 3 < -- BINGO

( // Pick a bingo number and stop
var num = 3;
// ~rolling.stop;
// ~setVal.value(9, 1, 0); // Stop
	~setVal.value(0, num, 0);
~screens[0].sendMsg("/eventById", 2, num);

~setMode.value(11);;
~setVal.value(0, num);
)




(
~setMode.value(11); // Countdown;
~count = 5;
~setVal.value(0, ~count);
)
~count = ~count - 1; ~setVal.value(0, ~count); // Manaul

(
{
	(30*5).do{
		|i|
		~setVal.value(13, i.linlin(0, 30*5, 0, 2));
		(1/30).wait;
		}
}.fork;
)

~setVal.value(13, 0, 1);