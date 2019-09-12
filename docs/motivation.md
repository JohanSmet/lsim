# Why did I build LSim?

You might be asking yourself: Why the h#$! did you spend hours and hours building LSim when there are more functional alternatives already available? And that is a perfectly valid question. To provide a satisfying answer we have to go to the beginning and tell a (not so) little story.

**tl;dr**: I wanted to write a script to test an adder circuit and things spiraled a bit out of control. But I had fun and learned a lot along the way.

I've been a software engineer for more than 20 years, depending on when you start counting. I've always had an interest in how the hardware worked but mostly on a higher level, not at the electronic component level. I knew about caches, instruction pipelines, branch predictors and how to parallelize a `fdiv` instruction with integer instructions on a Pentium CPU to optimize your perspective correct texture mapper. Yeah, showing my age here a little.

Like many recently I got bitten by the retro-bug and started tinkering around with games consoles and computers from my youth. I have a special fondness for the coin-op arcade machines.  Circuit boards for these are readily available but are either very expensive or broken. So you end up staring at a bunch of 74-series logic chips with a logic probe in hand trying to figure out why half of the sprites aren't drawn properly on the screen... Time to learn more about digital logic circuit design and debugging.

I started following online tutorials and courses and building circuits with Logisim (and derivatives). It's an awesome resource but when I built a 32-bit adder circuit I couldn't find an easy way of testing all possible inputs to verify the correct behavior of the entire circuit.

So I thought I'll write a small program that can load the Logisim circuit,  run the simulation and take input that I can control procedurally. So the first version of LSim was born: a C++ library (with unit tests of course) and Python bindings to script the test fixtures. I didn't support all the features of Logisim but enough to run the kind of circuits I was building at the time.

For more complicated circuits it was hard to debug LSim and see what was going wrong. So I wrote a graphical program to display the circuit and step through the simulation. This was ever only intended to be a visualization, not an editor. The way Logisim stores the circuits makes it hard to load them without a lot of inside information. So it just approximated the position of the components and replaced the wires with splines.

As you can imagine this became an indecipherable mess rather quickly. So I thought, how hard can it be to turn the visualization into a basic editor? Famous last words ;-)

Most of the time on this project was spent working on `lsim_gui`. I ended rewriting LSim to better segregate the construction/editing of the circuit and the simulation. The simulation engine itself underwent a few major revisions.

Along the way, I not only learned a lot about digital (e.g. I now have a basic understanding of how flip-flops and binary counters work) but also about the way digital circuits work electrically to be able to simulate them properly.

As it stands now (in September 2019) LSim is able to simulate a basic 8-bit computer (inspired by Ben Eater's work [here](https://eater.net/8bit). It's probably capable of more but the UI is a major limiting factor here.

It's certainly possible to improve it further but for me, it's time to move on the other projects and challenges right now. 

