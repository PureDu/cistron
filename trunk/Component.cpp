
#include "Component.h"
#include "ObjectManager.h"


using namespace Cistron;


#include <sstream>
#include <iostream>

using std::stringstream;
using namespace std;


// constructor/destructor
Component::Component(string name) : fOwnerId(-1), fName(name), fDestroyed(false), fTrack(false) {
	static ComponentId IdCounter = 0;
	fId = ++IdCounter;
}
Component::~Component() {
}


// track activity in the log
/*void Component::trackMe(track) {
	fTrack = track;
}*/


// check for validity of the object
bool Component::isValid() {
	return fOwnerId >= 0 && fName.size() > 0 && !fDestroyed;
}



// get owner
ObjectId Component::getOwnerId() {
	return fOwnerId;
}
void Component::setOwner(ObjectId id) {

	// set the owner
	fOwnerId = id;

	// respond to ping requests
	requestMessage("ping", &Component::processPing);
}
void Component::setDestroyed() {
	fDestroyed = true;
}
bool Component::isDestroyed() {
	return fDestroyed;
}


// get id
ComponentId Component::getId() {
	return fId;
}
string Component::getName() {
	return fName;
}


// add another component to this object
void Component::addLocalComponent(Component *c) {
	ObjectManager->addComponent(getOwnerId(), c);
}


/**
 * REQUEST FUNCTIONS
 */

// message request function
void Component::requestMessage(string message, MessageFunction f) {

	// construct registered component
	RegisteredComponent reg;
	reg.callback = f;
	reg.required = false;
	reg.component = this;
	reg.trackMe = false;

	// construct component request
	ComponentRequest req;
	req.type = REQ_MESSAGE;
	req.name = message;

	// forward to object manager
	ObjectManager->registerGlobalRequest(req, reg);
}

// require a component in this object
void Component::requireComponent(string name, MessageFunction f) {

	// construct registered component
	RegisteredComponent reg;
	reg.callback = f;
	reg.required = true;
	reg.component = this;
	reg.trackMe = false;

	// construct component request
	ComponentRequest req;
	req.type = REQ_COMPONENT;
	req.name = name;

	// forward to object manager
	ObjectManager->registerLocalRequest(req, reg);
}

// register a component request
void Component::requestComponent(string name, MessageFunction f, bool local) {

	// construct registered component
	RegisteredComponent reg;
	reg.callback = f;
	reg.required = false;
	reg.component = this;
	reg.trackMe = false;

	// construct component request
	ComponentRequest req;
	req.type = REQ_COMPONENT;
	req.name = name;

	// forward to object manager
	if (local) ObjectManager->registerLocalRequest(req, reg);
	else ObjectManager->registerGlobalRequest(req, reg);
}

// request all components of one type
void Component::requestAllExistingComponents(string name, MessageFunction f) {

	// construct registered component
	RegisteredComponent reg;
	reg.callback = f;
	reg.required = false;
	reg.component = this;
	reg.trackMe = false;

	// construct component request
	ComponentRequest req;
	req.type = REQ_ALLCOMPONENTS;
	req.name = name;

	// forward to object manager
	ObjectManager->registerGlobalRequest(req, reg);
}



// get a request id
RequestId Component::getMessageRequestId(string name) {
	return ObjectManager->getRequestId(REQ_MESSAGE, name);
}



/**
 * MESSAGING FUNCTIONS
 */

// send a message
void Component::sendMessage(string msg, void *payload) {
	ObjectManager->sendGlobalMessage(msg, this, payload);
}
void Component::sendMessage(RequestId reqId, void *payload) {
	ObjectManager->sendGlobalMessage(reqId, this, payload);
}
void Component::sendLocalMessage(string msg, void *payload) {
	ObjectManager->sendMessageToObject(msg, this, fOwnerId, payload);
}
void Component::sendLocalMessage(RequestId reqId, void *payload) {
	ObjectManager->sendMessageToObject(reqId, this, fOwnerId, payload);
}
void Component::sendLocalMessage(RequestId reqId, Message msg) {
	ObjectManager->sendMessageToObject(reqId, msg, fOwnerId);
}
void Component::sendMessageToObject(ObjectId id, string msg, void *payload) {
	ObjectManager->sendMessageToObject(msg, this, id, payload);
}
void Component::sendMessageToObject(ObjectId id, RequestId reqId, void *payload) {
	ObjectManager->sendMessageToObject(reqId, this, id, payload);
}
void Component::sendMessageToObject(ObjectId id, RequestId reqId, Message msg) {
	ObjectManager->sendMessageToObject(reqId, msg, id);
}



// called when added to an object
void Component::addedToObject() {
	// does nothing by default
}


// register a unique name
bool Component::registerName(string s) {
	return ObjectManager->registerName(fOwnerId, s);
}
ObjectId Component::getObjectId(string name) {
	return ObjectManager->getObjectId(name);
}


// destroy this component
void Component::destroy() {
	ObjectManager->destroyComponent(this);
}


// output
ostream& operator<<(ostream& s, Component &v) {
	return s << "Component[" << v.getId() << "][" << v.getName() << "] owned by [" << v.getOwnerId() << "]";
};


// to string
string Component::toString() {
	stringstream ss;
	ss << (*this);
	return ss.str();
}


/**
 * PING & LOGGING
 */


// process ping
void Component::processPing(Message msg) {
	cout << *this << " PING" << endl;
}


// track a component request
void Component::trackComponentRequest(string name, bool local) {

	// get request id
	RequestId reqId = ObjectManager->getRequestId(REQ_COMPONENT, name);

	// start tracking
	ObjectManager->trackRequest(reqId, local, this);
}


// track a message request
void Component::trackMessageRequest(string message) {

	// get request id
	RequestId reqId = ObjectManager->getRequestId(REQ_MESSAGE, message);
cout << "Tracking message request " << message << endl;
	// start tracking
	ObjectManager->trackRequest(reqId, false, this);
}