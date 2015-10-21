# What is Cistron? #

Cistron is a component-based programming framework targeted at game development. The philosophy behind this programming paradigm is well explained on the following website:

http://cowboyprogramming.com/2007/01/05/evolve-your-heirachy/

In Cistron, objects (such as a player, an opponent, a wall, the main game engine) are composed of components, which interact with each other through messages. For example, the main game engine might send a draw message to all other components, and components which subscribed to this message will receive the command and draw themselves on screen.

Cistron is designed as a light-weight, fast and flexible framework for use in environments where these frameworks are often not considered because of the overhead they cause through their message interactions.

# Features #
  * An easy-to-use interface
  * A portable, platform-independent framework
  * Message sending with very little overhead
  * Component creation/destruction detection