#include <iostream>
#include <string>
#include <cctype>       // needed for tolower() - makes menu case-insensitive
using namespace std;

//  Request Node to hold one customer's service request details
struct RequestNode
{
    string firstName;
    string lastName;
    string email;
    string description;
    string status;  //  the queue. Status always starts as "open" by default.
    RequestNode* next;  //  the 'next' pointer links it to the node behind it in

    RequestNode()
    {
        firstName   = "";
        lastName    = "";
        email       = "";
        description = "";
        status      = "open";   // every new request opens as "open"
        next        = nullptr;
    }
};

//  Stack Node wraps a full RequestNode copy so we can store cancelled
//  requests in a LIFO stack for recovery purposes
struct StackNode
{
    RequestNode data;
    StackNode*  next;

    StackNode()
    {
        next = nullptr;
    }
};

//  GLOBAL POINTER DECLARATIONS
RequestNode* front    = nullptr;  //  front / rear to manage the active request queue
RequestNode* rear     = nullptr;
StackNode*   stackTop = nullptr;  //  stackTop to manage the cancelled request stack


//  pushToStack - copies a RequestNode into a new
//  StackNode and places it on top of the cancelled stack.
//  Called whenever a request gets cancelled.
void pushToStack(RequestNode data)
{
    StackNode* freshNode  = new StackNode();
    freshNode->data       = data;
    freshNode->next       = stackTop;
    stackTop              = freshNode;
}

//  createRequest - gathers all five fields from the agent, sets status to
//  "open", and slots the new node at the back of the queue.
void createRequest()
{

    RequestNode* newNode = new RequestNode();

    cout << endl;
    cout << "--------- NEW SERVICE REQUEST ---------" << endl;
    cout << "First name         : ";
    cin  >> newNode->firstName;

    cout << "Last name          : ";
    cin  >> newNode->lastName;

    cout << "Email address      : ";
    cin  >> newNode->email;

    cin.ignore();   // flush leftover newline before getline

    cout << "Request description: ";
    getline(cin, newNode->description);

    // status and next are already set by the constructor
    // but we make it explicit here for clarity
    newNode->status = "open";
    newNode->next   = nullptr;

    // --- queue insertion ---
    if (front == nullptr)
    {
        // very first request - both pointers land on this node
        front = newNode;
        rear  = newNode;
    }
    else
    {
        // link new node to the tail, then shift rear forward
        rear->next = newNode;
        rear       = newNode;
    }

    cout << endl;
    cout << "Request logged. Request is open and waiting." << endl;
}

//  updateRequestStatus - finds a request by email and lets the agent pick a new status
//  Stops at the first match - emails are unique.
void updateRequestStatus()
{

    cout << endl;
    cout << "--------- UPDATE REQUEST STATUS ---------" << endl;

    if (front == nullptr)
    {
        cout << "The request queue is empty. Nothing to update." << endl;
        return;
    }

    string targetEmail;
    cout << "Enter the customer email: ";
    cin  >> targetEmail;

    RequestNode* ptr = front;

    while (ptr != nullptr)
    {
        if (ptr->email == targetEmail)
        {

            cout << "Request found. Current status: " << ptr->status << endl;
            cout << "What should the new status be?" << endl;
            cout << "  1 - open"      << endl;
            cout << "  2 - resolved"  << endl;
            cout << "  3 - cancelled" << endl;
            cout << "Pick a number  : ";

            int pick;
            cin >> pick;

            if      (pick == 1) ptr->status = "open";
            else if (pick == 2) ptr->status = "resolved";
            else if (pick == 3) ptr->status = "cancelled";
            else
            {
                cout << "That number is not on the list. Status left unchanged." << endl;
                return;
            }

            cout << "Done. Status is now set to: " << ptr->status << endl;
            return;
        }
        ptr = ptr->next;
    }

    // if we get here, the email was never found
    cout << "No request linked to " << targetEmail << ". Double-check the email and try again." << endl;
}

//  cancelRequest - removes a node from the active queue by email, marks it
//  cancelled, copies it to the stack, then frees the memory.
void cancelRequest()
{

    cout << endl;
    cout << "--------- CANCEL REQUEST ---------" << endl;

    if (front == nullptr)
    {
        cout << "Queue is empty. There is nothing here to cancel." << endl;
        return;
    }

    string targetEmail;
    cout << "Enter the customer email to cancel: ";
    cin  >> targetEmail;

    RequestNode* current = front;
    RequestNode* prev    = nullptr;

    while (current != nullptr)
    {
        if (current->email == targetEmail)
        {

            current->status = "cancelled";
            pushToStack(*current);   // save a copy before deletion

            // --- pointer adjustment based on node position ---
            if (prev == nullptr)
            {
                // target was sitting at the front of the queue
                front = current->next;
                if (front == nullptr) rear = nullptr;   // queue is now empty
            }
            else
            {
                // target was somewhere in the middle or at the rear
                prev->next = current->next;
                if (current == rear) rear = prev;       // rear needs to step back
            }

            delete current;
            cout << "Request removed from the queue and pushed to the recovery stack." << endl;
            return;
        }

        prev    = current;
        current = current->next;
    }

    cout << "Could not find a request for " << targetEmail << ". Check the email and retry." << endl;
}

// displayAllRequests - traverses the entire queue and prints every node, including
//  the raw memory addresses for tracing pointer links.
void displayAllRequests()
{

    cout << endl;
    cout << "--------- ALL REQUESTS IN QUEUE ---------" << endl;

    if (front == nullptr)
    {
        cout << "The queue is empty. No requests have been logged yet." << endl;
        return;
    }

    RequestNode* ptr  = front;
    int          num  = 1;

    while (ptr != nullptr)
    {
        cout << "------------------------------------------" << endl;
        cout << "Request #    : " << num                                    << endl;
        cout << "Name        : " << ptr->firstName << " " << ptr->lastName << endl;
        cout << "Email       : " << ptr->email                             << endl;
        cout << "Description : " << ptr->description                       << endl;
        cout << "Status      : " << ptr->status                            << endl;
        cout << "Node Addr   : " << ptr                                    << endl;
        cout << "Next Addr   : " << ptr->next                              << endl;

        ptr = ptr->next;
        num++;
    }

    cout << "------------------------------------------" << endl;
}

//  searchRequest - looks for a specific email in the queue and, when found,
//  shows the record alongside its node and next-pointer memory addresses.
void searchRequest()
{

    cout << endl;
    cout << "--------- SEARCH REQUESTS ---------" << endl;

    if (front == nullptr)
    {
        cout << "Nothing to search. The queue is empty." << endl;
        return;
    }

    string targetEmail;
    cout << "Email to search for: ";
    cin  >> targetEmail;

    RequestNode* ptr = front;

    while (ptr != nullptr)
    {
        if (ptr->email == targetEmail)
        {
            cout << endl;
            cout << "Request located." << endl;
            cout << "------------------------------------------" << endl;
            cout << "Name        : " << ptr->firstName << " " << ptr->lastName << endl;
            cout << "Email       : " << ptr->email                             << endl;
            cout << "Description : " << ptr->description                       << endl;
            cout << "Status      : " << ptr->status                            << endl;
            cout << "Node Addr   : " << ptr                                    << endl;
            cout << "Next Addr   : " << ptr->next                              << endl;
            return;
        }
        ptr = ptr->next;
    }

    cout << "Searched the whole queue. No request matches " << targetEmail << "." << endl;
}

// displayResolvedRequests - filters the queue and prints only nodes whose status
//  equals "resolved". Tells the agent if none exist.
void displayResolvedRequests()
{

    cout << endl;
    cout << "--------- RESOLVED REQUESTS ---------" << endl;

    if (front == nullptr)
    {
        cout << "Queue is empty. No resolved request to show." << endl;
        return;
    }

    bool         found = false;
    RequestNode* ptr   = front;

    while (ptr != nullptr)
    {
        if (ptr->status == "resolved")
        {
            cout << "------------------------------------------" << endl;
            cout << "Name        : " << ptr->firstName << " " << ptr->lastName << endl;
            cout << "Email       : " << ptr->email       << endl;
            cout << "Description : " << ptr->description << endl;
            cout << "Status      : " << ptr->status      << endl;
            found = true;
        }
        ptr = ptr->next;
    }

    if (!found)
    {
        cout << "No requests have been marked as resolved yet." << endl;
    }
    else
    {
        cout << "------------------------------------------" << endl;
    }
}

// displayCancelledRequests - shows every entry in the cancelled stack, most
//  recent first (top of stack). Stack is not modified.
void displayCancelledRequests()
{

    cout << endl;
    cout << "--------- CANCELLED REQUESTS (STACK) ---------" << endl;

    if (stackTop == nullptr)
    {
        cout << "The recovery stack is empty. No cancelled requests on record." << endl;
        return;
    }

    StackNode* ptr = stackTop;

    while (ptr != nullptr)
    {
        cout << "------------------------------------------" << endl;
        cout << "Name        : " << ptr->data.firstName << " " << ptr->data.lastName << endl;
        cout << "Email       : " << ptr->data.email       << endl;
        cout << "Description : " << ptr->data.description << endl;
        cout << "Status      : " << ptr->data.status      << endl;
        ptr = ptr->next;
    }

    cout << "------------------------------------------" << endl;
}

// recoverCancelledRequest - pops the top of the cancelled stack, creates a brand-new
//  node with status "open", and appends it to the rear of the active queue so it gets processed in turn.
void recoverCancelledRequest()
{

    cout << endl;
    cout << "--------- RECOVER CANCELLED REQUEST ---------" << endl;

    if (stackTop == nullptr)
    {
        cout << "Recovery stack is empty. Nothing to bring back." << endl;
        return;
    }

    // grab top of stack and build a fresh queue node from it
    StackNode*   popped  = stackTop;
    RequestNode* newNode = new RequestNode();

    newNode->firstName   = popped->data.firstName;
    newNode->lastName    = popped->data.lastName;
    newNode->email       = popped->data.email;
    newNode->description = popped->data.description;
    newNode->status      = "open";     // restored request reopens as a fresh request
    newNode->next        = nullptr;

    // pop the stack
    stackTop = stackTop->next;
    delete popped;

    // place the recovered node at the back of the active queue
    if (front == nullptr)
    {
        front = newNode;
        rear  = newNode;
    }
    else
    {
        rear->next = newNode;
        rear       = newNode;
    }

    cout << "Request for " << newNode->firstName << " " << newNode->lastName
         << " has been reopened and placed at the end of the queue." << endl;
}


// storeCancelledRequests - scans the active queue for any node already marked
//  "cancelled", moves those nodes to the stack, and removes them from the live queue.
void storeCancelledRequests()
{

    cout << endl;
    cout << "--------- STORE CANCELLED REQUESTS ---------" << endl;

    if (front == nullptr)
    {
        cout << "Nothing in the queue to scan." << endl;
        return;
    }

    bool         moved   = false;
    RequestNode* current = front;
    RequestNode* prev    = nullptr;

    while (current != nullptr)
    {
        if (current->status == "cancelled")
        {

            pushToStack(*current);
            moved = true;

            RequestNode* toRemove = current;

            // re-link the queue around the node being removed
            if (prev == nullptr)
            {
                front   = current->next;
                current = front;
                if (front == nullptr) rear = nullptr;
            }
            else
            {
                prev->next = current->next;
                if (current == rear) rear = prev;
                current    = prev->next;
            }

            delete toRemove;

        }
        else
        {
            prev    = current;
            current = current->next;
        }
    }

    if (moved)
    {
        cout << "All request marked cancelled have been shifted to the recovery stack." << endl;
    }
    else
    {
        cout << "Went through the whole queue. No cancelled requests were found." << endl;
    }
}

int main()
{

    char input;
    char choice;

    do
    {
        cout << endl;
        cout << "=========================================" << endl;
        cout << "   DataWave Solutions Ltd               " << endl;
        cout << "   Customer Service Request System      " << endl;
        cout << "=========================================" << endl;
        cout << "  A. Create new request                 " << endl;
        cout << "  B. Update request status              " << endl;
        cout << "  C. Cancel request                     " << endl;
        cout << "  D. Display all requests               " << endl;
        cout << "  E. Search for request                 " << endl;
        cout << "  F. Display resolved requests          " << endl;
        cout << "  G. Display cancelled requests         " << endl;
        cout << "  H. Recover cancelled request          " << endl;
        cout << "  I. Store cancelled request            " << endl;
        cout << "  J. Exit program                       " << endl;
        cout << "=========================================" << endl;
        cout << "Enter option: ";
        cin  >> input;

        // convert to lowercase so A and a both work
        choice = tolower(input);

        switch (choice)
        {
        case 'a':
            createRequest();
            break;
        case 'b':
            updateRequestStatus();
            break;
        case 'c':
            cancelRequest();
            break;
        case 'd':
            displayAllRequests();
            break;
        case 'e':
            searchRequest();
            break;
        case 'f':
            displayResolvedRequests();
            break;
        case 'g':
            displayCancelledRequests();
            break;
        case 'h':
            recoverCancelledRequest();
            break;
        case 'i':
            storeCancelledRequests();
            break;
        case 'j':
            cout << endl;
            cout << "Program terminated. Thank you for using our system! " << endl;
            break;
        default:
            cout << endl;
            cout << "'" << input << "' is not a valid option. Pick a letter between A and J." << endl;
        }

    }
    while (choice != 'j');

    return 0;
}
