p = SerialPort.listDevices
p = SerialPort.new("/dev/tty.usbserial-01A7ECB2", 230400, crtscts: true)
p.close

(
x.stop; // Color
x = {
	inf.do{
		|i|
		var msg = (0xFF!6) ++ [0x06] ++ [[1,1,1], [1,1,1]].wrapAt(i.asInteger) ++ [101,110,100];
		// var msg = (0xFF!6) ++ [0x08, 0x00, 0x00] ++ (1.0.rand + 0.1).asBytes ++ [101,110,100];
		// msg.postln;
		p.putAll(msg);
		0.5.wait;
	}
}.fork;
)

(
var id = 0;
var range = 1.5;
var offset = 0.25;
var freq = 0.1;
var waveTableID = 1;
var msg;
msg = (0xFF!6) ++ [0x08, 0x03, id] ++ (range.asFloat).asBytes ++ [101,110,100];
p.putAll(msg);
msg = (0xFF!6) ++ [0x08, 0x02, id] ++ (offset.asFloat).asBytes ++ [101,110,100];
p.putAll(msg);
msg = (0xFF!6) ++ [0x08, 0x00, id] ++ (freq.asFloat).asBytes ++ [101,110,100];
p.putAll(msg);
msg = (0xFF!6) ++ [0x08, 0x04, id] ++ [waveTableID] ++ [101,110,100];
p.putAll(msg);
// msg = (0xFF!6) ++ [0x06] ++ [0x01, 0x01, 0x01] ++ [101,110,100];
// p.putAll(msg);
)

( // Env
~envTest = {
var msg;
var a = 5.0;
var s = 10.0;
var r = 240.0;
var b = 1.0;
msg = (0xFF!6) ++ [0x08, 0x05] ++ a.asBytes ++ s.asBytes ++ r.asBytes ++ b.asBytes ++ [101,110,100];
p.putAll(msg);
};
~envTest.value
// msg = (0xFF!6) ++ [0x08, 0x00, 0] ++ (0.1 + 1.0.rand.asFloat).asBytes ++ [101,110,100];
// p.putAll(msg);
)
( // Set addValue for brightness
var msg;
var addValue = 0.2;
msg = (0xFF!6) ++ [0x08, 0x06] ++ addValue.asBytes ++ [101,110,100];
p.putAll(msg);
)

l =
[
	[[0x7C,0xDF,0xA1,0x03,0x9A,0x47], 0x01, 0], // 0
	[[0x7C,0xDF,0xA1,0x03,0x9A,0x51], 0x01, 1], // 1
	[[0x7C,0xDF,0xA1,0x03,0x9A,0x54], 0x01, 2], // 2
	[[0x7C,0xDF,0xA1,0x03,0x9A,0x3F], 0x01, 3], // 3
]

// Set Mode RAW
(
var msg = (0xFF!6) ++ [0x10] ++ [0x00] ++ [101,110,100];
p.putAll(msg);
)

