.. WorldEngine C++ documentation master file, created by
   sphinx-quickstart on Fri Mar 12 23:25:47 2021.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

WorldEngine C++ - based on the best open-source world generator
===============================================================

WorldEngine C++ is based on WorldEngine by Bret Curtis and Federico Tomassetti. The C++ project grew out of a desire to link a native WorldEngine library into an application, instead of bringing in the Python runtime. The goal of WorldEngine C++ is to maintain as much functionality and compatibility as possible. While random number generation differs between this and the original project resulting in new worlds, the application produces compatible world files and images using the same algorithms. You can find the original project here:

- https://github.com/Mindwerks/worldengine
- https://worldengine.readthedocs.io/en/latest/

WorldEngine has the goal to be the best open-source world generator available. It is based on the simulation of several physical phenomens.

Our philosophy is to build a tool very flexibile, which can be used from the command line, in a GUI or as a component inside other programs.

WorldEngine can be very easy to use: you click a button and you get a nice world generated for you.

It can also be very complex: do you want to set specific temperature or humidity ranges for your world? Specify a strange ratio for your map? You can.

We think that most advanced users could want to use WorldEngine as a part of larger tool-chain: you give some draft of your world to WorldEngine, it performs some extra simulations for you, making your world more realistic, and then you feed the result from WorldEngine into another tool of yours or perhaps you refine manually your results.

So WorldEngine can be used in different ways. Let's see how.

Contents:

.. toctree::
   install
   cli
   gui
   biomes
   scenarios
   contributing
