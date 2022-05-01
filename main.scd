s.boot;
// t = TIYCS.new("192.168.1.2").gui
t = TIYCS.new("127.0.0.1").gui
t.gui
~j = JoniskMain.new.gui;
~j.sequence = [	4,5,9,10]; // Den Dolder
~j.triggerNextInSeq();
~j.jonisks[0].synth.set(\amp, 0)

// Tank-sequence
MIDIIn.doNoteOnAction(1, 14, 25, 127);
MIDIIn.doNoteOnAction(1, 14, 26, 127);
MIDIIn.doNoteOnAction(1, 14, 27, 127);
MIDIIn.doNoteOnAction(1, 14, 28, 127);
MIDIIn.doNoteOnAction(1, 14, 29, 127);
MIDIIn.doNoteOnAction(1, 14, 29, 127);
MIDIIn.doNoteOnAction(1, 14, 30, 127);
MIDIIn.doNoteOnAction(1, 14, 31, 127);

MIDIIn.doNoteOnAction(1, 1, 20, 127); // Commercial 0

MIDIIn.doNoteOnAction(1, 14, 36, 127); // Jonisk seq (ARP)

MIDIIn.doNoteOnAction(1, 14, 35, 127); // Alarm on
MIDIIn.doNoteOffAction(1, 14, 35, 127); // Alarm off

MIDIIn.doNoteOnAction(1, 14, 34, 127); // Captain scene

MIDIIn.doNoteOnAction(1, 14, 19, 127); // Shopping (when Commercials scene is active)