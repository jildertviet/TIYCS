~joniskValues = Bus.alloc(\control, s, 16 * 1 + 16 * 4); // brightness, RGBW

// All Jonisks blink
~joniskBlink = {Out.kr(~joniskValues.subBus(0, 16), LFPulse.kr(1)!16)}.play;
~joniskBlink.free();

// All Jonisks one color
(
~setJoniskColor = {
	|rgbw = #[0,0,0,0], white=0|
	if(rgbw.isArray == false, {rgbw = (rgbw.asArray.at((0..2)) * 254).asInteger; rgbw = rgbw ++ [white]});
	rgbw.postln;
	~joniskValues.subBus(16, 4*16).setn(rgbw.lace(4*16))
};
)

~setJoniskColor.value(Color.green, 254); // Green with full white
~setJoniskColor.value([0,0,0,0]);