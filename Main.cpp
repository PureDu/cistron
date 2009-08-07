
/**
 * Example program using the component framework.
 * In the component framework, an object is represented by a collection of components,
 * who interact with each other through messages.
 * An Object traditionally corresponds to what you would define as a class in a traditional OO environment.
 * However, in this framework, the object is further split into smaller components.
 * 
 * This example implements a company, its personnel and the government serving taxes.
 * In this example, a person is represented by an Object, which can contain several
 * components that are all part of the same person, like one or more jobs.
 * The company is represented by another object.
 * The government is represented by yet another object.
 */

#include "Cistron.h"

using namespace Cistron;


#include <string>
#include <iostream>
using namespace std;



/**
 * Job component. Each person is assigned a job by the company.
 */
class Job: public Component {

	public:

		// constructor/destructor
		Job() : Component("Job") {
			/**
			 * In the constructor, the component is not added to an object yet
			 * therefore, it is not yet part of the component framework,
			 * and cannot request or send messages.
			 */
		};
		virtual ~Job() {};

		// we are added to an object, and thus to the component system
		void addedToObject() {
			/*
			 * From this point onwards, the component is part of the framework, and can
			 * request and send messages.
			 */


			/**
			 * Request a message.
			 * Whenever a Fire message is sent to this object, this function will be called
			 * So that the Job can destroy itself.
			 */
			requestMessage("Fire", &Job::fire);
		}

		// set/get the salary
		void setSalary(int s) { fSalary = s; }
		int getSalary() { return fSalary; }


	private:


		// we are fired from this job
		
		void fire(Message msg) {
			/**
			 * Message is a struct containing the following fields:
			 *    - type: One of CREATE,DESTROY,MESSAGE
			 *            Must be MESSAGE in this case, because the message originated from a requestMessage call
			 *    - sender: A void pointer to the Component who sent the message.
			 *    - p: A void pointer pointing to an optional payload for the message. This can be "added" to the
			 *         message by the sender of the message.
			 */

			// just destroy ourselves
			destroy();
		}


		// salary
		int fSalary;
};


/**
 * Person component. Has a job.
 */
class Person: public Component {

	public:

		// constructor - age is fixed at creation time
		Person(string name, int age) : Component("Person"), fName(name), fAge(age) {};
		virtual ~Person() {};

		// get the age of the person
		int getAge() { return fAge; }


		// we are added to an object, and thus to the component system
		void addedToObject() {
			cout << fName << " with age " << fAge << " was added to the system." << endl;

			/**
			 * request creation & destruction messages for a particular component
			 * note that, if there are already components of this type in the system,
			 * CREATE messages will be generated for these persons when the request is processed
			 *
			 * Note that we want to know of our own jobs - but not of other people's jobs.
			 * That is why we set the local argument to "true". This means we only request
			 * job components that exist in the same Object as we do.
			 */
			requestComponent("Job", &Person::processJob, true);

			// when the year changes, we want to update our age, so we request NextYear events
			requestMessage("NextYear", &Person::nextYear);

		}


	private:


		// we received a job - make a happy dance
		void processJob(Message msg) {
			/**
			 * Message is a struct containing the following fields:
			 *    - type: One of CREATE,DESTROY,MESSAGE
			 *            Must be CREATE or DESTROY in this case, because the message originated from
			 *            a requestComponent call.
			 *    - sender: A void pointer to the Component who was just created, or is about to be destroyed (deleted).
			 *    - p: not used for CREATE or DESTROY messages.
			 */

			Job *job = (Job*)msg.sender;

			// we received a new job! hooray!
			if (msg.type == CREATE) {
				cout << fName << " (age " << fAge << ") received a new job with salary " << job->getSalary() << endl;
			}
			else {
				cout << fName << " (age " << fAge << ") lost his job with salary " << job->getSalary() << endl;
			}
		}

		// we receive a next year message - update the age, and let everyone know that our age changed
		void nextYear(Message msg) {

			// first, we update our age
			++fAge;
			cout << fName << " had a birthday and is now " << fAge << " years old" << endl;

			/**
			 * Send a message to everyone that we aged.
			 * There are many different functions available to make it easy to send messages.
			 * Basically there are three ways to send a message:
			 *    - sendLocalMessage: send a message to all the components in THIS object who have requested this message type.
				  - sendMessage: send a message to all the components in the system who have requested this message type.
				  - sendMessageToObject: send a message to a particular object (defined by its id), which can differ from this one.
			 *
			 * Next to these three methods, there are several overloaded versions of each,
			 * based on whether you want to give a payload, etc. The different basic versions are:
			 *
			 * sendLocalMessage("MessageName", payloadPointer); // payload is not provided
			 * sendLocalMessage("MessageName", payloadPointer); // payload provided
			 *
			 * sendMessage("MessageName"); // payload is not provided
			 * sendMessage("MessageName", payloadPointer); // payload provided
			 *
			 * sendMessageToObject(objectId, "MessageName"); // payload is not provided
			 * sendMessageToObject(objectId, "MessageName", payloadPointer); // payload provided
			 * objectId is obtained by calling getOwnerId() on a component.
			 *
			 * In this case, we just want to announce the whole world of our birthday.
			 */
			sendMessage("Birthday");
		}


		// age
		int fAge;

		// name
		string fName;

};




/**
 * the company class. Wants to know who works in this company, and assigns jobs to newcomers.
 */
class Company: public Component {
	
	public:

		// constructor/destructor
		Company() : Component("Company") {};
		virtual ~Company() {};


		// when added to an object
		void addedToObject() {

			/**
			 * request creation & destruction messages for a particular component
			 * note that, if there are already components of this type in the system,
			 * CREATE messages will be generated for these persons when the request is processed
			 *
			 * Note that we want to know all persons added to the system,
			 * thus the optional third "local" argument is not provided.
			 * This will alert the Company of all Person components who are added,
			 * regardless of the object they belong to.
			 */
			requestComponent("Person", &Company::processPerson);

			// request all currently existing components of one type,
			// but don't request messages about components created afterwards
			// this is not necessary because we called requestComponent, 
			// so existing components of the Person type will be already reported
			// requestAllComponents("Person", &Company::processPerson);


			// request a birthday message
			// fire employees who are too old and give an extra job to people who turned 50
			requestMessage("Birthday", &Company::processBirthday);
		}


	private:


		// process component of type 2
		void processPerson(Message msg) {

			// get the person
			Person *person = (Person*)msg.sender;

			// new person added to the company
			if (msg.type == CREATE) {

				// create job component
				Job *job = new Job();

				// salary
				int salary = person->getAge() * 10000;

				// give the person a job, salary is based on his age
				job->setSalary(salary);

				// add the job to the same object that the person belongs to
				ObjectManager->addComponent(person->getOwnerId(), job);


				// really old people receive a second job - bonus work for more salary
				if (person->getAge() >= 50) {
					Job *extraJob = new Job();
					extraJob->setSalary(50000);
					ObjectManager->addComponent(person->getOwnerId(), extraJob);
				}
			}

		}



		// process birthday
		void processBirthday(Message msg) {

			// get the person whose birthday it is
			Person *person = (Person*)msg.sender;

			// person is too old - fire him!
			if (person->getAge() == 65) {

				// we only want to fire this person, so we send a message to the object this person belongs to
				sendMessageToObject(person->getOwnerId(), "Fire");
			}


			// person just turned 50 - give him a bonus job!
			if (person->getAge() == 50) {
				Job *extraJob = new Job();
				extraJob->setSalary(50000);
				ObjectManager->addComponent(person->getOwnerId(), extraJob);
			}
		}

};

/*
 * Finally, the government component.
 * This is where the advantage of the component based architecture comes in!
 * The government is only interested in how many money is made in its country, not by who it is earned.
 * therefore, it does not have to know anything about the Person component to function - it only interacts with Job.
 * Additionally, Jobs do not have to be tied to Persons either - for example, one could implement jobs for companies,
 * which are paid for by the government. Then there would be an object containing a Company and Job component, but not a
 * Person component. Yet, the government will still be able to compute the total earned income.
 * We also make the government responsible for maintaining the calendar - someone has to do it.
 */
class Government: public Component {

	public:

		// constructor/destructor
		Government() : Component("Government"), fTotalEarnedIncome(0) {};
		virtual ~Government() {};

		// we advance the calendar by one year
		void advanceCalendar() {

			// send message to everyone that we're in a new fiscal year
			cout << "New fiscal year!" << endl;
			sendMessage("NextYear");

			// announce new total
			cout << "Government announces total earned salary at this year: " << fTotalEarnedIncome << endl;
		}

		// when it is added to the system, we request all jobs that exist and will exist
		void addedToObject() {

			// request all job components
			requestComponent("Job", &Government::processJob);
		}


	private:

		// the total money earned
		int fTotalEarnedIncome;


		// process new and lost jobs
		void processJob(Message msg) {

			// get the job
			Job *job = (Job*)msg.sender;

			// new job
			if (msg.type == CREATE) {
				// add to the total earned income
				fTotalEarnedIncome += job->getSalary();
			}

			// lost job - type cannot be MESSAGE so must be DESTROY
			else {
				// substract from total earned income
				fTotalEarnedIncome -= job->getSalary();
			}
		};



};


/**
 * We now play around with the objects created.
 */
int main(char **args) {

	// first, initialize the object manager
	ObjectManagerInit();



	// first, create a new object
	ObjectId p1Id = ObjectManager->createObject();

	// this object is now empty and contains no components
	// we create a person component and add it to the newly created object
	Person *p1 = new Person("Walter", 43);
	ObjectManager->addComponent(p1Id, p1);

	// we now create a company in its own separate object
	ObjectId companyId = ObjectManager->createObject();
	Company *company = new Company();
	ObjectManager->addComponent(companyId, company);
	
	// the company will now assign a job to person 1

	// add two more persons, one who will soon retire, and one who will get his bonus job soon

	// old person
	ObjectId p2Id = ObjectManager->createObject();
	Person *p2 = new Person("Bob", 62);
	ObjectManager->addComponent(p2Id, p2);

	// soon to turn 50
	ObjectId p3Id = ObjectManager->createObject();
	Person *p3 = new Person("Peter", 48);
	ObjectManager->addComponent(p3Id, p3);

	// now the company has assigned 4 jobs, 1 to Walter, 1 to Peter and 2 to Bob

	// now, we add the government, which will keep track of the total earned income in our mini universe
	ObjectId governmentId = ObjectManager->createObject();
	Government *government = new Government();
	ObjectManager->addComponent(governmentId, government);

	// finally, we advance the calendar by a couple of years, and see what happens
	government->advanceCalendar();
	government->advanceCalendar();
	government->advanceCalendar();
	government->advanceCalendar();


	// we're all done - clean up the object manager
	ObjectManagerDestroy();

	return 0;
}