This project is an addition to DiGiT's work on a project called AOKTS, or Age of Kings Trigger Studio, 
which allows Age of Empires II players to edit custom scenarios they make
at a much faster pace than if they were to edit with the default scenario editor
that is built in to the game. I figured that by making his code compilable
in a modern tool, people could use the awesome power of C++ to edit custom scenarios
at a lightning speed. If you are unfamiliar with DiGiT's original work, and are a fan of
Age of Empires II, visit here: http://aok.heavengames.com/blacksmith/showfile.php?fileid=5858

This is a note by Alex Leung on creating custom scripts for DiGiT's AoKTS.
I TAKE NO CREDIT FOR DiGiT's work.

I was making a really big scenario and somewhere around 600 triggers I got fet
up with repetitive tasks and decided to look into the source of this program, which
I had been using for its trigger/effect/condition copying and the duplicate for all
players function.

This project compiled in VS2005, but not in VS2012 at first, so I did a bit of
tweaking by bringing in the zlib and expat sources and resolving some linkage
issues. So, now, the project compiles in VS2012.

What I have done is added an extra MENUITEM to the File menu on AoKTS (compare the
original 1.0.1 to the .exe in this folder) called Custom Task which will run
whatever code is included in custscript.cpp (also in this folder). The entryPoint
is the customScript void method, so you can code based on that. Use the code that 
I have already written to begin to familiarize yourself with the wonderful methods
and classes written by DiGiT. Also be sure to explore the files written by DiGiT
himself (I had to edit the common.rc and aokts.cpp files to create this
functionality) if you want to better understand what methods to use.

I hope you know C/C++ and I hope this helps you create scenarios at a exponential
pace (it really should with a powerful scripting language now at you hand!).

I only added the one menu item instead of completely changing around the whole 
application so you can have your own code and still use all of DiGiT's original
UI-based scenario-editing functions. I encourage you to change the application
as you wish and implement the features you wish were there, I am curious what kind
of changes people will make (please share them)!
JUST BE SURE TO GIVE CREDIT TO THOSE WHO DESERVE IT!
Enjoy!