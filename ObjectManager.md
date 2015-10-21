# ObjectManager #

ObjectManager is the main class of the framework. You need to create an instance of this class before you can use Cistron. The ObjectManager is used to create objects and add components to objects. All the other functionality of Cistron is made available to the components through an interface in Component.

The ObjectManager object should only be used directly to create an initial set of objects and components and add them to the framework. After initialization, all the interaction should go through the Component interface.


## Functions ##

### `ObjectId createObject()` ###

Creates a new object, and returns a unique identifier for this object. From now on, components can be added to the object.

### `void addComponent(ObjectId objId, Component*)` ###

Add a component to an object. Calls the `addedToObject()` function of the component.