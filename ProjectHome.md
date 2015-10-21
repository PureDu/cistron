# Description #

Cistron is a component-based programming framework targeted at game development. The philosophy behind this programming paradigm is well explained on the following website:

http://cowboyprogramming.com/2007/01/05/evolve-your-heirachy/

Alternatively, I have recently done a presentation on this subject. The slides can be found below. The presentation contains examples, illustrates the difference with classical OO programming and gives a list of benefits and possible issues.

http://www.codeximperium.be/stuff/Component-based%20programming.pdf

In Cistron, objects (such as a player, an opponent, a wall, the main game engine) are composed of components, which interact with each other through messages. For example, the main game engine might send a draw message to all other components, and components which subscribed to this message will receive the command and draw themselves on screen.

Cistron is designed as a light-weight, fast and flexible framework for use in environments where these frameworks are often not considered because of the overhead they cause through their message interactions. It does not depend on any libraries besides the C++ standard library, so it is easy to integrate in any project.

# Features #
  * An easy-to-use interface
  * A portable, platform-independent framework
  * Message sending with very little overhead (almost as fast as direct function calls)
  * Component creation/destruction detection and easy processing


# Why use Cistron? #

Cistron might help you out if you're having one of the following problems:

  * You are working with lots of game objects in a deep inheritance hierarchy, and you end up having to put a LOT of functionality in the root object of the tree, because it's shared by so many different subclasses. **Cistron** solves this issue by splitting objects up in components that each implement a particular functionality. Objects are composed of these components depending on the functionality they need.
  * You have trouble organizing communication between different game objects. Maybe you solved this by creating some sort of managing object that keeps a list of all the objects, and acts as a mediator between them. Or maybe you determined a hierarchy between different game object types, and the one higher up in the hierarchy leads the communication with the ones lower down. A typical case in which this problem often occurs is collision detection. **Cistron** allows you to have neutral communication directly between objects, without a hierarchy and without another class linking the two objects together.
  * You have a "Game" class which contains a lot of lists, each for different object types, in order to keep track of which object should be drawn, which should be updated, which should receive mouse or keyboard input, and so on. **Cistron** solves this issue by allowing components to subscribe to messages, which are then broadcast by the game object and automatically sent only to those components who are interested in them. Different components (different classes) can subscribe and receive the same message, and process it in different ways, without the need for a shared superclass interface.

# Get started #

To get started, read the [documentation wiki](Documentation.md).
If you plan on using Cistron in one of your projects, or if you have questions concerning the project, please contact [the author](mailto:Karel.Crombecq@telenet.be).