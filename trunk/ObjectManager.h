
#ifndef INC_OBJECTMANAGER
#define INC_OBJECTMANAGER


#include "CSingleton.h"
#include "Object.h"


#include <hash_map>
#include <list>
#include <string>
#include <boost/format.hpp>

using std::list;
using std::string;
using std::pair;
using stdext::hash_map;


namespace Cistron {


// the object manager manages all object entities, and performs communication between them
class ObjectMgr: public CSingleton<ObjectMgr> {

	public:

		// constructor/destructor
		ObjectMgr();
		virtual ~ObjectMgr();

		// init/destroy of the singleton
		inline static void init() {
			fObjectMgrInstance = new ObjectMgr();
		};
		inline static void destroy() {
			delete fObjectMgrInstance;
		};


		// create a new object
		ObjectId createObject();

		// add a new component to an object
		void addComponent(ObjectId, Component*);

		// destroy object
		void destroyObject(ObjectId);

		// destroy a component
		void destroyComponent(Component*);

		// finalize an object, resolving the required components
		void finalizeObject(ObjectId);


		// register a unique name for an object
		bool registerName(ObjectId, string name);

		// get the id based on the unique name identified
		ObjectId getObjectId(string name);



		/**
		 * REQUEST MESSAGES
		 */

		// register global requests
		void registerGlobalRequest(ComponentRequest, RegisteredComponent reg);

		// register a local request
		void registerLocalRequest(ComponentRequest, RegisteredComponent reg);


		/**
		 * SENDING MESSAGES
		 */

		// send global messages
		inline void sendGlobalMessage(string msg, Component *component, void *payload) {
			sendGlobalMessage(getExistingRequestId(REQ_MESSAGE, msg), Message(MESSAGE, component, payload));
		}
		inline void sendGlobalMessage(RequestId reqId, Component *component, void *payload) {
			sendGlobalMessage(reqId, Message(MESSAGE, component, payload));
		}
		void sendGlobalMessage(RequestId reqId, Message msg);

		// send local messages to another object
		inline void sendMessageToObject(string msg, Component *component, ObjectId id, void *payload) {
			fObjects[id]->sendMessage(getRequestId(REQ_MESSAGE, msg), Message(MESSAGE, component, payload));
		}
		inline void sendMessageToObject(RequestId reqId, Component *component, ObjectId id) {
			fObjects[id]->sendMessage(reqId, Message(MESSAGE, component));
		}
		inline void sendMessageToObject(RequestId reqId, Component *component, ObjectId id, void *payload) {
			fObjects[id]->sendMessage(reqId, Message(MESSAGE, component, payload));
		}
		inline void sendMessageToObject(string name, Message msg, ObjectId id) {
			fObjects[id]->sendMessage(getRequestId(REQ_MESSAGE, name), msg);
		}
		inline void sendMessageToObject(RequestId reqId, Message msg, ObjectId id) {
			fObjects[id]->sendMessage(reqId, msg);
		}

		// ask for a request id
		RequestId getRequestId(ComponentRequestType, string name);

		/**
		 * LOGGING
		 */

		// track a request
		void trackRequest(RequestId, bool local, Component*);

		// get request name
		inline string getRequestById(ComponentRequestType type, RequestId reqId) {
			return fIdToRequest[type][reqId];
		}


	private:

		// the singleton instance
		static ObjectMgr *fObjectMgrInstance;

		/**
		 * REQUEST IDS
		 */

		// request id counter
		RequestId fRequestIdCounter;

		// mapping from a request to a unique id that identifies the request
		hash_map<string, RequestId> fRequestToId[2];

		// mapping from request id to the original request name
		hash_map<RequestId, string> fIdToRequest[2];

		// get an existing request id
		RequestId getExistingRequestId(ComponentRequestType, string name);


		/**
		 * LOCKING MECHANISM
		 */

		// activate/release request lock
		// locks are activated when the current request id is being iterated on, to avoid conflicts with iterators
		void activateLock(RequestId);
		void releaseLock(RequestId);

		// number of active locks
		int fNLocks;

		// a request lock contains whether this request is at the moment locked
		// a request is locked if currently, a message is being processed for this request
		struct RequestLock {
			bool locked;
			list<pair<ComponentRequest, RegisteredComponent> > pendingLocalRequests;
			list<pair<ComponentRequest, RegisteredComponent> > pendingGlobalRequests;
			RequestLock() : locked(false) {};
		};

		// list of locked request id's
		vector<RequestLock> fRequestLocks;

		// list of pending destroyals
		list<Component*> fDeadComponents;

		/**
		 * OBJECTS
		 */

		// id counter
		ObjectId fIdCounter;

		// list of objects with their id's
		vector<Object*> fObjects;

		// mapping of objects to their unique name identified
		hash_map<string, ObjectId> fObjectNameToId;

		/**
		 * REQUESTS
		 */


		// vector of global requests
		vector<list<RegisteredComponent> > fGlobalRequests;

		// list of required components which still need to be processed
		hash_map<ObjectId, list<string> > fRequiredComponents;

		// list of component requests, by component id
		hash_map<ComponentId, list<ComponentRequest> > fRequestsByComponentId;


		/**
		 * ERROR PROCESSING
		 */

		// error function
		void error(boost::format str);



};


// some defines
#define ObjectManager ObjectMgr::getSingletonPtr()

// initialization
extern void ObjectManagerInit();
extern void ObjectManagerDestroy();


};


#endif
