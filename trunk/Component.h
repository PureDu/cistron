
#ifndef INC_COMPONENT
#define INC_COMPONENT


#include <string>
#include <map>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <ostream>

using std::map;
using std::string;
using std::ostream;


namespace Cistron {


// object & component id
typedef int ObjectId;
typedef int ComponentId;


// a request ID
typedef int RequestId;

// type of component requests
enum ComponentRequestType {
	REQ_COMPONENT = 0,
	REQ_MESSAGE = 1,
	REQ_ALLCOMPONENTS = 2
};

// a request
struct ComponentRequest {
	ComponentRequestType type;
	string name;
	ComponentRequest(ComponentRequestType t, string n) : type(t), name(n) {};
	ComponentRequest() {};
};




// event type
enum MessageType {
	CREATE,
	DESTROY,
	MESSAGE
};


// an event
class Component;
struct Message {
	MessageType type;
	Component *sender;
	void *p;
	Message(MessageType t) : type(t) {};
	Message(MessageType t, Component *c) : type(t), sender(c) {};
	Message(MessageType t, Component *c, void *payload) : type(t), sender(c), p(payload) {};
};




// component function
typedef boost::function<void(Message)> MessageFunction;


// a component registered for an event (message or component creation/destruction)
struct RegisteredComponent {
	Component *component;
	MessageFunction callback;
	bool required;
	bool trackMe;
};


// a generic component
class Component {

	public:

		// constructor/destructor
		Component(string name);
		virtual ~Component();



		// function called when the component is added to an object
		virtual void addedToObject();

		// add another component to this object
		void addLocalComponent(Component*);

		// register a unique name for the object
		bool registerName(string s);

		// get object id
		ObjectId getObjectId(string name);


		/**
		 * REQUEST FUNCTIONS
		 */

		// message request function
		void requestMessage(string message, MessageFunction);

		// require a component in this object
		void requireComponent(string name, MessageFunction);

		// register a component request
		void requestComponent(string name, MessageFunction, bool local = false);

		// request all components of one type
		void requestAllExistingComponents(string name, MessageFunction);

		// request a request id of a message
		RequestId getMessageRequestId(string name);

		/**
		 * FANCY TEMPLATED REQUEST FUNCTIONS
		 */

		// message request function
		template<class T>
		void requestMessage(string message, void (T::*f)(Message));

		// require a component in this object
		template<class T>
		void requireComponent(string name, void (T::*f)(Message));

		// register a component request
		template<class T>
		void requestComponent(string name, void (T::*f)(Message), bool local = false);

		// request all components of one type
		template<class T>
		void requestAllExistingComponents(string name, void (T::*f)(Message));


		/**
		 * MESSAGING FUNCTIONS
		 */

		// send a message
		void sendMessage(string msg, void *payload = 0);
		void sendMessage(RequestId id, void *payload = 0);
		void sendMessageToObject(ObjectId id, string msg, void *payload = 0);
		void sendMessageToObject(ObjectId id, RequestId reqId, void *payload = 0);
		void sendMessageToObject(ObjectId id, RequestId reqId, Message msg);
		void sendLocalMessage(string msg, void *payload = 0);
		void sendLocalMessage(RequestId reqId, void *payload = 0);
		void sendLocalMessage(RequestId reqId, Message msg);

		/**
		 * IMPLEMENTED REQUESTS & LOGGING
		 */
		void processPing(Message);

		void trackComponentRequest(string name, bool local = false);
		void trackMessageRequest(string message);
		




		// get owner
		ObjectId getOwnerId();

		// get id
		ComponentId getId();

		// destroy this component
		void destroy();
		bool isDestroyed();

		// valid component?
		bool isValid();

		// get the name of the component
		string getName();

		// to string
		string toString();

	private:

		// set owner
		void setOwner(ObjectId id);

		// set destroyed
		void setDestroyed();

		// object id
		ObjectId fOwnerId;

		// component it
		ComponentId fId;

		// name of the component
		string fName;

		// destroyed
		bool fDestroyed;

		// track this component in the log
		bool fTrack;

		// object manager is our friend
		friend class ObjectMgr;

};


/**
 * TEMPLATED REQUEST FUNCTIONS
 */

// message request function
template<class T>
void Component::requestMessage(string message, void (T::*f)(Message)) {
	requestMessage(message, boost::bind(f, dynamic_cast<T*>(this), _1));
}

// require a component in this object
template<class T>
void Component::requireComponent(string name, void (T::*f)(Message)) {
	requireComponent(name, boost::bind(f, dynamic_cast<T*>(this), _1));
}

// register a component request
template<class T>
void Component::requestComponent(string name, void (T::*f)(Message), bool local) {
	requestComponent(name, boost::bind(f, dynamic_cast<T*>(this), _1), local);
}

// request all components of one type
template<class T>
void Component::requestAllExistingComponents(string name, void (T::*f)(Message)) {
	requestAllExistingComponents(name, boost::bind(f, dynamic_cast<T*>(this), _1));
}


};


// output
ostream& operator<<(ostream& s, Cistron::Component &v);


#endif
