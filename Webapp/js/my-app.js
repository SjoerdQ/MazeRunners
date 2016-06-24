// Initialize your app
var myApp = new Framework7();

// Export selectors engine
var $$ = Dom7;

// Add view
var mainView = myApp.addView('.view-main', {
    // Because we use fixed-through navbar we can enable dynamic navbar
    dynamicNavbar: true,
    swipePanel: 'left'
    
});

// Callbacks to run specific code for specific pages, for example for About page:
myApp.onPageInit('about', function (page) {
    // run createContentPage func after link was clicked
    
    $$('.create-page').on('click', function () {
        createContentPage();
    });
});

// Create a client instance
client = new Paho.MQTT.Client("iot.eclipse.org",80,"WebObserver");

setTimeout(connectToBroker,250);
function connectToBroker() {
    // connect the broker
    client.connect(options);
}

// Generate dynamic page
var dynamicPageIndex = 0;
function createContentPage() {
	mainView.router.loadContent(
        '<!-- Top Navbar-->' +
        '<div class="navbar">' +
        '  <div class="navbar-inner">' +
        '    <div class="left"><a href="#" class="back link"><i class="icon icon-back"></i><span>Back</span></a></div>' +
        '    <div class="center sliding">Dynamic Page ' + (++dynamicPageIndex) + '</div>' +
        '  </div>' +
        '</div>' +
        '<div class="pages">' +
        '  <!-- Page, data-page contains page name-->' +
        '  <div data-page="dynamic-pages" class="page">' +
        '    <!-- Scrollable page content-->' +
        '    <div class="page-content">' +
        '      <div class="content-block">' +
        '        <div class="content-block-inner">' +
        '          <p>Here is a dynamic page created on ' + new Date() + ' !</p>' +
        '          <p>Go <a href="#" class="back">back</a> or go to <a href="services.html">Services</a>.</p>' +
        '        </div>' +
        '      </div>' +
        '    </div>' +
        '  </div>' +
        '</div>'
    );
	return;
}

// MQTT Websockets stuff

// When players pages loads
myApp.onPageInit('players', function (page) {
var subscribeOptions = {
    qos: 0,  // QoS
    invocationContext: {foo: true},  // Passed to success / failure callback
    onSuccess: onPlayerSuccessCallback,
    onFailure: onPlayerFailureCallback,
    timeout: 10
};
    client.subscribe("ESEiot/mazerunner/posPlayer",subscribeOptions);
});

myApp.onPageBack('players', function (page) {
 //   client.disconnect();
 console.log("onPageBack!");
 client.unsubscribe("ESEiot/mazerunner/posPlayer");
});

function onPlayerSuccessCallback() {
    console.log("subscribed succesfully");
}

function onPlayerFailureCallback() {
    console.log("subscribe fallure");
}

myApp.onPageInit('gamemap', function (page) {
    var subscribeOptions = {
    qos: 0,  // QoS
    invocationContext: {foo: true},  // Passed to success / failure callback
    onSuccess: onPlayerSuccessCallback,
    onFailure: onPlayerFailureCallback,
    timeout: 10
};
    client.subscribe("ESEiot/mazerunner/fullPlayField",subscribeOptions);
    document.getElementById('GameMapRaw').innerHTML = "Connected! Start the main gamemap game, a player and a demon to start!";    
});

myApp.onPageBack('gamemap', function (page) {
    client.unsubscribe("ESEiot/mazerunner/fullPlayField");
});

// set callback handlers
client.onConnectionLost = onConnectionLost;
client.onMessageArrived = onMessageArrived;
var options = {
    onSuccess:onConnect,
    onFailure:doFail
}



// called when the client connects
function onConnect() {
    // Once a connection has been made, make a subscription and send a message.
    console.log("onConnect");
   // client.subscribe("ESEiot/mazerunners/posDemons");
    
    //message = new Paho.MQTT.Message("admin logIn Player 1");
    //message.destinationName = "/ESEiot/mazerunners/admin";
    //client.send(message); 
    myApp.addNotification({
        title: 'Connected!',
        message: 'Connected to the MQTT broker.'
    });
   // document.getElementById('playerList').innerHTML = "connected to broker! Please wait for a message...";
  }
  
function doFail(e){
    console.log(e);
    myApp.addNotification({
        title: 'Connecting failed!',
        message: 'Error while connecting to the MQTT broker. '+e
    });
    //document.getElementById('playerList').innerHTML = "Error while connecting: "+e;
  }
  
  // called when the client loses its connection
function onConnectionLost(responseObject) {
    if (responseObject.errorCode !== 0) {
      console.log("onConnectionLost:"+responseObject.errorMessage);
    myApp.addNotification({
        title: 'Connection lost! 😢',
        message: 'The connection to the broker is lost. Close this message to try again. <br> Error: '+responseObject.errorMessage,
       // setTimeout(function () { location.reload }, 5000);
         onClose: function () {
            location.reload();
        }
    });      
        //  document.getElementById('playerList').insnerHTML = "connection to broker lost!";
    }
}

// called when a message arrives
function onMessageArrived(message) {
        console.log("onMessageArrived: "+message.payloadString);
        //console.log("topic: "+message.destinationName);
        var topicname = message.destinationName;
    //if (topicname= "ESEiot/mazerunners/posPlayer") {
       // document.getElementById('playerList').innerHTML = message.payloadString;
        document.getElementById('GameMapRaw').innerHTML = message.payloadString;    
    // } 
    // else if (topicname = "ESEiot/mazerunners/playField") {
        // document.getElementById('playerList').innerHTML = message.payloadString;
    // }   
    
    
 }