(
var slidersY = 30;
var slWidth = 350;
var slHeight = 20;
var yOffset= 20;
var x = 400;
w = Window.new("TIYCS", Rect(400, 0, 1000, 1000)).front;

~screenBrSlider = EZSlider.new(w, Rect(x, slidersY, slWidth, slHeight), label:"Brightness");
~screenBrSlider.action_({ |ez| ~setVal.value(2, ez.value * 255, -1)}).value_(1);

~joniskBrightness = EZSlider.new(w, Rect(x, slidersY + (yOffset*1), 350, 20), label:"J br ");
~joniskBrightness.action_({ |ez| ~setBrightness.value(-1, ez.value)}).valueAction_(0.01);

g = EZSlider.new(w, Rect(x, slidersY + (yOffset*2), slWidth, slHeight), label:"J noise");
g.action_({
	|ez|
	j.do{|e| e.set(\noiseMul, ez.value)}; // Glow Jonisk
}).value_(0);

g = EZSlider.new(w, Rect(x, slidersY + (yOffset*3), slWidth, slHeight), label:"Lag", controlSpec: ControlSpec(0.01, 3, 'lin'));
g.action_({
	|ez|
	j.do{|e| e.set(\lagTime, ez.value)};
}).valueAction_(0.1);
~envLevelSlider = EZSlider.new(w, Rect(x, slidersY + (yOffset*4), slWidth, slHeight), label:"Env br", controlSpec: ControlSpec(0.01, 3, 'lin'));
~envLevelSlider.action_({
	|ez|
	~envLevel = ez.value;
}).value_(0.1);
g = EZSlider.new(w, Rect(x, slidersY + (yOffset*5), slWidth, slHeight), label:"Env sus", controlSpec: ControlSpec(0.01, 3, 'lin'));
g.action_({
|ez|
	~env[1] = ez.value;
}).value_(0.2);
~envReleaseSlider = EZSlider.new(w, Rect(x, slidersY + (yOffset*6), slWidth, slHeight), label:"Env rel", controlSpec: ControlSpec(0.01, 3, 'lin'));
~envReleaseSlider.action_({
	|ez|
	~env[2] = ez.value;
}).value_(0.1);
~heightSlider = EZSlider.new(w, Rect(x, slidersY + (yOffset*7), slWidth, slHeight), label:"Fly hight", controlSpec: ControlSpec(0, 2, 'lin'));
~heightSlider.action_({
	|ez|
	~setVal.value(13, ez.value, -1);
}).value_(2);
~flySpeedSlider = EZSlider.new(w, Rect(x, slidersY + (yOffset*8), slWidth, slHeight), label:"Fly speed", controlSpec: ControlSpec(0.2, 10, 'lin', 0.001));
~flySpeedSlider.action_({
	|ez|
	~setVal.value(14, ez.value, -1);
}).value_(1);


~buttons = Array.fill(32, {
	|i|
	var x = (i.asInteger) % 4;
	var y = (i / 4).asInteger;
	Button.new(w, Rect(30 + (x * 80), 30 + (y * 80), 80, 80)).action_({|v| v.postln});
});

~buttons[0].action_({
		~setMode.value(0,0);
	~setMode.value(12, 1);
	~increaseStartIntensity.stop; ~increaseStartIntensity = {
		(30*30).do{
			|i|
			~setVal.value(12, 1 + i.linlin(0, 900, 0, 2).postln);
				(1/30).wait;
			}
		}.fork;
}).string_("Start");

~buttons[1].action_({
	~currentInstruction = 0;
	~setMode.value(1, -1); //
	~setVal.value(3, 0, -1);
	~bobbieMute = false; // Here, just in case I skip button[0]
}).string_("Instructions");

~buttons[2].action_({
	~currentInstruction = ~currentInstruction + 1;
	~setVal.value(3, ~currentInstruction, 1);
	// ~setVal.value(3, ~currentInstruction, 2);
}).string_("Next");


~buttons[3].action_({
	~setMode.value(11, -1); // Countdown;
	~count = 15;
	~setVal.value(0, ~count, -1);
}).string_("Count start");
~buttons[4].action_({
	~count = ~count - 1; ~setVal.value(0, ~count, -1); // Manaul
}).string_("Next count");


~buttons[5].action_({
	~setMode.value(3, 0); // Waveform
}).string_("Waveform");


~buttons[6].action_({
	~setMode.value(4, -1); // Stars
}).string_("Stars");

~buttons[7].action_({
	~setMode.value(5, 0); // Vluchtinfo
	(
		~updateRoute = {
			200.do{
				|i|
				~setVal.value(8, i/200, 0);
				0.9.wait;
			}
		}.fork;
	);
	~updateRoute.stop;
	~setVal.value(4, true); // Flight info
}).string_("Vluchtinfo");

~buttons[8].action_({
	~setMode.value(8, -1); ~screens.do{|screen|screen.sendMsg("/eventById", 1, 2)};
	~setVal.value(4, false, 0); // Flight info off!!!
}).string_("Reclame 0");

~buttons[9].action_({
	~setMode.value(6, -1); ~screens.do{|screen|screen.sendMsg("/eventById", 1, 0)};
}).string_("Reclame 1");

~buttons[10].action_({
	~setMode.value(7, -1); ~screens.do{|screen|screen.sendMsg("/eventById", 1, 1)};
}).string_("Reclame 2");



~buttons[11].action_({
	~setMode.value(9, -1); ~screens.do{|screen|screen.sendMsg("/setValueById", 1, -1)}; 	~setMode.value(4, 1); // Stars // Benzine
}).string_("Benzine");

~bRedBg = 0;
~buttons[12].action_({
	~bRedBg = (~bRedBg + 1) % 2;
	~bRedBg.postln;
	~setVal.value(10, ~bRedBg, -1);
}).string_("Benzine blink");
~buttons[13].action_({
	(
		~tanken.stop; ~tanken = {
			(30*15).do{
				|i|
				~screens.do{|screen|screen.sendMsg("/setValueById", 1, i.linlin(0, (30*15), 0, 150))};
				((1/30)).wait;
			}
		}.fork;
	)
}).string_("Vullen");
~buttons[14].action_({
	(
		~setMode.value(16, -1); // Terug in schip
		~imageIndex = 0;
		~setVal.value(11, ~imageIndex, -1)
	)
}).string_("Terug in schip");
~buttons[15].action_({
	~imageIndex = ~imageIndex + 1; ~setVal.value(11, ~imageIndex, -1); // Entree Jelte, en meer
}).string_("Next");
~buttons[16].action_({
	~setMode.value(2, 0); // Captain
	~setMode.value(3, 1);
}).string_("Captain");

~buttons[17].action_({ // BINGO
	~setMode.value(13,0);
	~setMode.value(20,1);
	~rolling.stop; ~setVal.value(9, 1, 0); // Stop bingo
	~setRGB.value(-1, Color.blue.asArray);
	~setBrightness.value(-1, 0.5); // Bingo
	5.do{var id = 16.rand; ~setBrightness.value(id, 0.2); ~setRGB.value(id, Color.new255(255, 192, 203).asArray);};
	~bingoIndex = 0;
	~setVal.value(15, 0, -1);
}).string_("Bingo");

~buttons[18].action_({
	~rolling.stop; ~rolling = {inf.do{~setVal.value(9, [0,1, 0.5].choose, 0); 0.5.wait}}.fork; // Start rolling
}).string_("Bingo roll");
~buttons[19].action_({
	~rolling.stop; ~setVal.value(9, 1, 0); // Stop
}).string_("Stop roll");



~buttons[20].action_({
	var num = ~bingoCode[~bingoIndex];
	~screens[0].sendMsg("/eventById", 2, num); // Remove the ball
	~setMode.value(17,1);
	~setVal.value(0, num,1);
	~bingoIndex = ~bingoIndex+ 1;
}).string_("Next num");

~buttons[21].action_({
	~bingoIndex = 0;
	~bingoCode = [4, 5, 6, 9, 3, -1];
	~setMode.value(14, 0); ~codeLen = 0; // CODE
	~setVal.value(6, ~codeLen, 0)
}).string_("Code invoer");
~buttons[22].action_({
	~codeLen = ~codeLen + 1; ~setVal.value(6, ~codeLen, 0); //++  Increase
}).string_("Code ++");
~buttons[23].action_({
	~setMode.value(15); ~setVal.value(7, 0); // Autopilot
	~setVal.value(0, -1,1); // Hide number if still there
}).string_("Auto P");
~buttons[24].action_({
	~updateAutoPilot .stop; ~updateAutoPilot = {101.do{|i|~setVal.value(7, i);0.18.wait}}.fork;
}).string_("Start");

~buttons[25].action_({
	~setRGB.value(-1, Color.white.asArray);
}).string_("White");

~buttons[26].action_({
	~setMode.value(11, -1); // Countdown;
	~count = 5;
	~setVal.value(0, ~count, -1);
}).string_("Count");
~buttons[27].action_({
~count = ~count - 1; ~setVal.value(0, ~count, -1); // Manaul
}).string_("Next");

~buttons[28].action_({
	~screenBrightness.value(0);
	~setBrightness.value(-1, 0);
	~bobbieMute = true;
}).string_("Black");

~buttons[29].action_({
~setMode.value(19, -1);
}).string_("Wordt verv");

~buttons[30].action_({
~setVal.value(15, 1, -1);
}).string_("Bingo rot bg");

);