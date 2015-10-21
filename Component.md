# Component #

Component is the class that every component has to derive from, to be included into the Cistron framework. Component serves as a gateway for communication with other objects. A component should only call functions of the Component class to request and send messages.

the ObjectManager class is _only_ used for adding new components to the framework. All other communication goes through Component and is processed internally. Below a summary and short explanation for all the functions of Component are described.

Components can be used in cout to get a readable description of the component, along with its id.

## Basic functionality ##

### `Component(string name)` ###

The constructor of Component must be called by the derived class with a unique name identifying this component. This should typically be the name of the class that is being created. To avoid problems, two different classes should never use the same component name.

Do not call any functions from Component from your own constructor. These can only first be called when the component is added to an object (see below).

### `virtual void addedToObject()` ###

This function is called when the component is added to an object, and thus is added to the framework. This is the first point at which a component can request or send messages, and can add other components.


### `RequestId getMessageRequestId(string msg)` ###

This function will return the unique request id corresponding to the given message `msg`. The RequestId can then subsequently be used and re-used to send messages of type `msg`, without performing the (relatively) expensive string to id lookup every time the message is sent.

If a message is sent many times by the same component, this component should get the RequestId once, store it, and use it every time the message is being sent, for a considerable speedup.


### `ComponentId getId()` ###

Get the (unique) id of this component.


### `ObjectId getOwnerId()` ###

Get the id of the object that this component belongs to.


### `void destroy()` ###

This will destroy the component. If this function is called from within a message callback function, the destruction is postponed until all the messages are done sending. When there are no more callbacks to call, the following actions will be performed in order:
  1. All requests registered for this component are removed. The component will not remove any further messages.
  1. The component is removed from the object it belonged to.
  1. A DESTROY message is sent to every component who requested CREATE/DESTROY messages for this component type.
  1. delete is called on the component.

Note that after destroy, the object is removed from memory, and no further actions should be taken in the function that called destroy.


## Component/Object Manipulation ##


### `ObjectId createObject()` ###

Creates a new object, and returns a unique identifier for this object. From now on, components can be added to the object.


### `void destroyObject(ObjectId)` ###

Destroy the object and all the components that belong to the object. Calls the `destroy()` of every component in order.


### `void addComponent(ObjectId objId, Component*)` ###

Add a component to an object. Calls the `addedToObject()` function of the component after succesful processing of the component.


### `void addLocalComponent(Component *c)` ###

Adds a new component to the object manager, in the same object as the current component. Short for `addComponent(getOwnerId(), c)`.

### `list<Component*> getComponents(string name, ObjectId id = -1)` ###

get all components of a given type, either from the local object (if the second argument is ommited) or from another object identified by the object id.


### `void finalizeObject(ObjectId)` ###

Finalizes the object. At this moment, all the requirements placed by components calling `requireComponent` are checked, and if some requirements are not met, the entire object is destroyed.


## Requests ##

### `void requestMessage(string msg, MessageFunction)` ###

Start listening to the message with name msg. From now on, all messages of this type that are sent, either globally, or to the object this component belongs to, are forwarded to the function that is provided. MessageFunction should be a member function (private or public) of the component.


### `void requestComponent(string componentName, MessageFunction, bool local = false)` ###

Start listening for creation or destruction messages for the component type componentName. Whenever a component of this type is added to the framework or is destroyed, the message function is called with a CREATE/DESTROY message.

The third optional `local` argument determines whether the function is called for every component of that type in the entire framework, or only for components of that type that belong to the same object.

If, at the moment of request, there are already components of this type in the framework, the message function is called one time for each component that already exists.


### `void requestAllExistingComponents(string componentName, MessageFunction)` ###

Call the message function for each component of type `componentName` that currently exists in the system. Will not call the message function if new components are added later, or if components are destroyed.

### `void requireComponent(string componentName, MessageFunction)` ###

This function can be called instead of requestComponent if one component depends of the existence of another component in the same object so much that is cannot function if this other component is not there.

When an object is finalized (see [ObjectManager](ObjectManager.md)), Cistron will check if all the requirements are met, and will destroy any component that does not have access to all the required components.

Besides this additional requirement, requireComponent(componentName, &A::fun) functions identical to `requestComponent(componentName, &A::fun, true)`, and will thus signal the creation and destruction of components of the given type after finalization.

## Message sending ##

### `sendMessage(string msg, void *payload = 0)` ###
### `sendMessage(RequestId id, void *payload = 0)` ###

Send a message of type msg to every component listening to this message. Can optionally provide a payload parameter which is passed along. This works with both the request id and the message name directly.


### `sendLocalMessage(string msg, void *payload = 0)` ###
### `sendLocalMessage(RequestId reqId, void *payload = 0)` ###
### `sendLocalMessage(RequestId reqId, Message msg)` ###

Send a message only to components belonging to the same object that are listening to this message. Can optionally provide a payload parameter which is passed along. This works with both the request id and the message name directly.

Finally, to make repeated message sending even faster, the message can be constructed in advance as well. Then the third version of the function can be used. Example code:

```
Message msg(MESSAGE, this, payload);
RequestId reqId = getMessageRequestId("Add");
for (int i = 0; i < 100000; ++i) {
    sendLocalMessage(reqId, msg);
}
```


### `void sendMessageToObject(ObjectId id, string msg, void *payload = 0)` ###
### `void sendMessageToObject(ObjectId id, RequestId reqId, void *payload = 0)` ###
### `void sendMessageToObject(ObjectId id, RequestId reqId, Message msg)` ###

These functions behave identically to sendLocalMessage, except for that the message is not sent to all components in the same object, but to all components in the given object.