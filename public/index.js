let componentName = "";

let waypoints = [];
let numWpt = 0;

// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {

    $('#databaseButtons').hide();
    $('#queryDiv').hide();
    $('#databaseTable').hide();
    
    // On page-load AJAX Example
    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/endpoint1',   //The server endpoint we are connecting to
        data: {
            stuff: "Value 1",
            junk: "Value 2"
        },
        success: function (data) {
            /*  Do something with returned object
                Note that what we get is an object, not a string, 
                so we do not need to parse it on the server.
                JavaScript really does handle JSONs seamlessly
            */
            $('#blah').html("On page load, received string '"+data.stuff+"' from server");
            //We write the object to the console to show that the request was successful
            console.log(data); 

        },
        fail: function(error) {
            // Non-200 return, do something with error
            $('#blah').html("On page load, received error from server");
            console.log(error); 
        }
    });

    // Event listener form example , we can use this instead explicitly listening for events
    // No redirects if possible
    $('#someform').submit(function(e){
        $('#blah').html("Form has data: "+$('#entryBox').val());
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            //Create an object for connecting to another waypoint
        });
    });


    // function needs to add multiple waypoints to the route
    $('#addRouteForm').submit(function(e){
        // $('#blah').html("Form has data: "+$('#entryBox').val());

        // console.log(componentFileName);

        
        

        console.log($("#addRouteNameInput").val());
        // console.log($("#addRouteLat").val());
        // console.log($("#addRouteLon").val());


        let strlat = ($('#addRouteLat').val());
        let strlon = ($('#addRouteLon').val());

        let wptObject = {};

        let lat = parseFloat(strlat);
        let lon = parseFloat(strlon);

        if (isNaN(strlat) || isNaN(strlon) || strlat === '' || strlon === ''){
            alert("Error: Enter a valid latitude and longitude.");
        }     
        else if (lat < -90 || lat > 90 || lon < -180 || lon > 180){
            alert("Error: Latitude must be between -90 and 90\nLongitude must be between -180 and 180");
        } else {

            numWpt++;

            wptObject.lat = lat;
            wptObject.lon = lon;

            // console.log(lat);
            // console.log(wptObject.lat);

            waypoints.push(wptObject);

            // console.log(waypoints);

            $('#waypointCountTracker').html("<p>" + numWpt +" waypoints added <p>");

            alert("Successfully added waypoint " + numWpt);
        }



        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        // $.ajax({
        //     // type: "get",
        //     // dataType: "json",
        //     // url: "/addRouteEndpoint",
        //     // data: {
        //     //     routeName: $("#addRouteNameInput").val(),
        //     //     fileName: componentFileName,
        //     //     oldName: oldCompName
        //     // },
        //     // success: function(data){
        //     //     console.log("success: " + data.filePath);
        //     // },
        //     // fail: function(error){
        //     //     console.log("failed to rename");
        //     //     console.log(error);
        //     // }
        // });
    });
    
    // $('#createGPXformdiv').submit(function(e){
    //     // $('#blah').html("Form has data: "+$('#entryBox').val());

    //     console.log("check submit");

    //     console.log($("#gpxNameInput").val());
    //     console.log($("#gpxCreatorInput").val());

    //     e.preventDefault();
    //     //Pass data to the Ajax call, so it gets passed to the server
    //     $.ajax({
    //         // type: "get",
    //         // dataType: "json",
    //         // url: "/createGPXEndpoint",
    //         // data: {
    //         //     routeName: $("#addRouteNameInput").val(),
    //         //     fileName: componentFileName,
    //         //     oldName: oldCompName
    //         // },
    //         // success: function(data){
    //         //     console.log("success: " + data.filePath);
    //         // },
    //         // fail: function(error){
    //         //     console.log("failed to rename");
    //         //     console.log(error);
    //         // }
    //     });
    // });


    $('#renameForm').submit(function(e){
        // $('#blah').html("Form has data: "+$('#entryBox').val());
        console.log($("#formGroupExampleInput").val());
        console.log(componentFileName);
        console.log(componentName);

        // console.log(arrayOfCompObj);

        let oldCompName = "";
        // go through this array and find the name
        arrayOfCompObj.forEach(function(comp){
            if (comp.compName == componentName){
                
                oldCompName = comp.name;
                
                
            }
        })

        // console.log(oldCompName);
        console.log(componentName);

        //go through file and change the name of route 1
        //

        // e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            type: "get",
            dataType: "json",
            url: "/renameEndpoint",
            data: {
                newName: $("#formGroupExampleInput").val(),
                fileName: componentFileName,
                oldName: oldCompName
            },
            success: function(data){
                alert("Success: Renamed " + componentName);
                console.log("success! ");
            },
            fail: function(error){
                console.log("failed to rename");
                console.log(error);
                alert("Error: failed to rename.");
            }
            // Create an object for connecting to another waypoint
        });
        // location.reload();
    });

    var fileTableString = "<table id=\"fileLogTable\" class=\"table table-bordered table-hover table-curved\"><thead class=\"thead-light\"><tr><th scope=\"col\">File Name <br>(click to download)</th> <th scope=\"col\">Version</th> <th scope=\"col\">Creator</th><th scope=\"col\">Number of Waypoints</th> <th scope=\"col\">Number of Routes</th> <th scope=\"col\">Number of Tracks</th> </tr>    </thead>";
    
    var dropdownString = "";
    // var formDropString = "<select class=\"form-control form-control-md\" id=\"fileSelectMenu\"> <option>Select File</option>";

    $.ajax({
        type: "get",
        dataType: "json",
        url: "/gpxAttributes",
        success: function (gpxData) {
            
            gpxData.forEach(function(data) {
                $("#emptyFile").remove();
                fileTableString+=("<tr>");

                fileTableString+=("<th scope=\"col\"><a href=\"../uploads/"+ data.fileName +"\" download \">"+ data.fileName+" </a> </th>");
                fileTableString+=("<th scope=\"col\">" + data.version+"</th>");
                fileTableString+=("<th scope=\"col\">"+data.creator+"</th>");
                fileTableString+=("<th scope=\"col\">"+data.numWaypoints+"</th>");
                fileTableString+=("<th scope=\"col\">"+data.numRoutes+"</th>");
                fileTableString+=("<th scope=\"col\">"+data.numTracks+"</th>");
                
                fileTableString+=("</tr>");

                // formDropString+= "<option value=\"" + data.fileName + "\">"+data.fileName+"</option>";
                
                dropdownString+=("<button onclick=\"dropdownButton(this)\" class=\"dropdown-item\" type=\"button\">"+data.fileName+"</button>");
            });

            // formDropString+="</select>";

            // fileTableString+=("</table>");
            $("#fileLogSec").html(fileTableString);
            $("#dropdownlist").html(dropdownString);

            // $("#fileSelect").html(formDropString);

            
        },
        fail: function (error) {
            alert("Error creating table from uploaded files.");
        }

    })

    // $.ajax({
    //     type: "get",
    //     dataType: "json",
    //     url: "/componentPanel",
    //     success: function (compData) {
            
    //         compData.forEach(function(data) {
    //         });
    
    //     },
    //     fail: function (error) {
            
    //     }
    
    // })

});

let componentFileName = "";
let componentOtherData;
let componentData;
let arrayOfCompObj = [];

// var componentTableString = "<table id=\"compTable\" class=\"table table-bordered table-hover table-curved\"><thead class=\"thead-light\"><tr><th scope=\"col\">Component</th><th scope=\"col\">Name</th><th scope=\"col\">Number of Points</th><th scope=\"col\">Length</th><th scope=\"col\">Loop</th></tr></thead>";
let renameButtonString = "";

let finalRenameButtons;

function addRouteFunc(){
    

    console.log(componentFileName);       

    console.log($("#addRouteNameInput").val());



    if (numWpt < 1){
        alert("Error: Must have at least one waypoint.");
    } 
    else {

        let rt = {};
        rt.name = $("#addRouteNameInput").val();

        console.log(waypoints);
        console.log(rt);

        $.ajax({
            type: "get",
            dataType: "json",
            url: "/addRouteEndpoint",
            data: {
                rt: rt,
                waypoints: waypoints,
                fileName: componentFileName,
            },
            success: function(data){
                console.log("success: " + data.routeStr);
            },
            fail: function(error){
                console.log("failed to rename");
                console.log(error);
                alert("Error: failed to rename.");
            }
        });

        numWpt = 0;
    }
    alert("Successfully added new route to " + componentFileName);

    location.reload();
    
}

function buttonthing(file){
    // console.log(file.innerHTML);

    $("#compTable tbody").remove();
    $("#renameForm").html("<p>Please choose a component</p><div id=\"renameButtonDiv\"><h4>No File Selected</h4></div>");

    var componentTableString = "<table id=\"compTable\" class=\"table table-bordered table-hover table-curved\"><thead class=\"thead-light\"><tr><th scope=\"col\">Component</th><th scope=\"col\">Name</th><th scope=\"col\">Number of Points</th><th scope=\"col\">Length</th><th scope=\"col\">Loop</th></tr></thead>";

    var otherDataButtonString = "";
    renameButtonString = "";

    arrayOfCompObj = [];

    $.ajax({
        type: "get",
        dataType: "json",
        url: "/componentPanel",
        success: function (gpxData) {

            
            gpxData.forEach(function(data) {
                // console.log("data name:" + data.name);
                componentTableString+="<tbody>";
                if (data.fileName === file.innerHTML){
                    // console.log("file: "+data.fileName);

                    arrayOfCompObj.push(data);

                    componentFileName = data.fileName;
                    // componentOtherData = data.otherData;

                    componentData = data;

                    componentTableString+=("<tr>");

                    componentTableString+=("<th scope=\"col\">" +data.compName+ "</th>");
                    componentTableString+=("<th scope=\"col\">" + data.name+ "</th>");
                    componentTableString+=("<th scope=\"col\">" +data.numPoints+ "</th>");
                    componentTableString+=("<th scope=\"col\">" +data.len+ "</th>");
                    componentTableString+=("<th scope=\"col\">" +data.loop+ "</th>");
                    
                    componentTableString+=("</tr>");

                    otherDataButtonString+="<button type=\"button\" onclick=\"dataButton(this)\" id=\"ComponentButton\" class=\"btn btn-primary Button\" data-dismiss=\"modal\">"+data.compName+"</button>";
                    renameButtonString+="<button type=\"button\" onclick=\"renameButton(this)\" id=\"ComponentButton\" class=\"btn btn-primary Button\" data-dismiss=\"modal\">"+data.compName+"</button>";
                }

            });

            componentTableString+="</tbody>";
            componentTableString+="</table>";
            $("#gpxViewPanel").html(componentTableString);
            $("#otherDataButtonsDiv").html(otherDataButtonString);
            $("#renameButtonDiv").html(renameButtonString);
            finalRenameButtons = renameButtonString;
            $("#compStatus").html("<p>viewing: "+componentFileName+"</p>");
            $("#addRouteFileSelected").html("<p>Adding route to: "+componentFileName+"</p>");
        },
        fail: function (error) {
            
        }
    
    })
    
    
}

async function storeFiles(){

    console.log($("#loginUserInput").val());
    console.log($("#loginPassInput").val());
    console.log($("#databaseInput").val());

    $.ajax({
        type: "get",
        dataType: "json",
        url: "/storeFilesEndpoint",
        data: {
            // username: loginUser,
            // password: loginPass,
            // database: databaseName
        },
        success: function(data){
            console.log("success! ");
            alert("Database has " + data.numFiles + " files, " + data.numRoutes + " routes, " + data.numPoints +" points");
        },
        error: function(e){
            alert(e.responseJSON.message);
        },
        fail: function(error){
            console.log("failed to create");
            console.log(error);
            alert("Error: failed to create.");
        }
        // Create an object for connecting to another waypoint
    });
}

async function clearDataButton(){
    // console.log($("#loginUserInput").val());
    // console.log($("#loginPassInput").val());
    // console.log($("#databaseInput").val());

    // let loginUser = $("#loginUserInput").val();
    // let loginPass = $("#loginPassInput").val();
    // let databaseName = $("#databaseInput").val();

    // if(loginUser === "" || loginPass === "" || databaseName === ""){
    //     alert("Enter a valid username, password and database.");
    // }

    $.ajax({
        type: "get",
        dataType: "json",
        url: "/clearDataEndpoint",
        data: {
            // username: loginUser,
            // password: loginPass,
            // database: databaseName
        },
        success: function(data){
            console.log("success! ");
            alert("cleared FILE, ROUTE, and POINT tables.\n");
            $('#loginModal').modal('hide');
            // $('#databaseButtons').show();
        },
        error: function(e){
            alert(e.responseJSON.message);
        },
        fail: function(error){
            console.log("failed to create");
            console.log(error);
            alert("Error: failed to create.");
        }
        // Create an object for connecting to another waypoint
    });

}

async function loginButton(){
    console.log($("#loginUserInput").val());
    console.log($("#loginPassInput").val());
    console.log($("#databaseInput").val());

    let loginUser = $("#loginUserInput").val();
    let loginPass = $("#loginPassInput").val();
    let databaseName = $("#databaseInput").val();

    // if(loginUser === "" || loginPass === "" || databaseName === ""){
    //     alert("Enter a valid username, password and database.");
    // }

    $.ajax({
        type: "get",
        dataType: "json",
        url: "/loginEndpoint",
        data: {
            username: loginUser,
            password: loginPass,
            database: databaseName
        },
        success: function(data){
            console.log("success! ");
            alert("Successfully logged into : " + loginUser + " with database: " + databaseName + "\nFILES: " + data.numFiles + "\nROUTES: " + data.numRoutes + "\nPOINTS: " + data.numPoints);
            $('#loginModal').modal('hide');
            $('#databaseButtons').show();
            $('#queryDiv').show();
            $('#databaseTable').show();
        },
        error: function(e){
            alert(e.responseJSON.message);
        },
        fail: function(error){
            console.log("failed to create");
            console.log(error);
            alert("Error: failed to create.");
        }
        // Create an object for connecting to another waypoint
    });

}

async function sortRoutesName(){

    let dbTableString = "<table id=\"dbTable\" class=\"table table-bordered table-hover table-curved\"><thead class=\"thead-light\">        <tr>            <th scope=\"col\">Route ID</th>            <th scope=\"col\">Name</th>            <th scope=\"col\">Length</th>            <th scope=\"col\">GPX ID</th>        </tr>    </thead>";

    $.ajax({
        type: "get",
        dataType: "json",
        url: "/sortAllRoutesNameEndpoint",
        data: {
            // username: loginUser,
            // password: loginPass,
            // database: databaseName
        },
        success: function(routes){

            console.log(routes);

            routes.forEach(function(rt){
                dbTableString+=("<tr>");
                dbTableString+=("<th scope=\"col\">" + rt.id+"</th>");
                dbTableString+=("<th scope=\"col\">"+rt.name+"</th>");
                dbTableString+=("<th scope=\"col\">"+rt.len+"</th>");
                dbTableString+=("<th scope=\"col\">"+rt.gpxID+"</th>");
                dbTableString+=("<tr>");
            })

            dbTableString+=("</table>");

            $("#databaseTable").html(dbTableString);
            
            console.log("Executed Query Sort! ");
        },
        error: function(e){
            alert(e.responseJSON.message);
        },
        fail: function(error){
            console.log("failed to create");
            console.log(error);
            alert("Error: failed to create html table.");
        }
        // Create an object for connecting to another waypoint
    });
}

async function sortRoutesLength(){
    let dbTableString = "<table id=\"dbTable\" class=\"table table-bordered table-hover table-curved\"><thead class=\"thead-light\">        <tr>            <th scope=\"col\">Route ID</th>            <th scope=\"col\">Name</th>            <th scope=\"col\">Length</th>            <th scope=\"col\">GPX ID</th>        </tr>    </thead>";

    $.ajax({
        type: "get",
        dataType: "json",
        url: "/sortAllRoutesLengthEndpoint",
        data: {
            // username: loginUser,
            // password: loginPass,
            // database: databaseName
        },
        success: function(routes){

            console.log(routes);

            routes.forEach(function(rt){
                dbTableString+=("<tr>");
                dbTableString+=("<th scope=\"col\">" + rt.id+"</th>");
                dbTableString+=("<th scope=\"col\">"+rt.name+"</th>");
                dbTableString+=("<th scope=\"col\">"+rt.len+"</th>");
                dbTableString+=("<th scope=\"col\">"+rt.gpxID+"</th>");
                dbTableString+=("<tr>");
            })

            dbTableString+=("</table>");

            $("#databaseTable").html(dbTableString);
            
            console.log("Executed Query Sort! ");
        },
        error: function(e){
            alert(e.responseJSON.message);
        },
        fail: function(error){
            console.log("failed to create");
            console.log(error);
            alert("Error: failed to create html table.");
        }
        // Create an object for connecting to another waypoint
    });
}

function createGPXbutton(){
    // console.log("check");

    console.log($("#gpxNameInput").val());
    console.log($("#gpxCreatorInput").val());

    let docName = $("#gpxNameInput").val();
    let docCreator = $("#gpxCreatorInput").val();
    let docVersion = 1.1;

    if(docName === "" || docCreator === ""){
        alert("Enter a valid Name and Creator");
    } 
    else if (docName.endsWith(".gpx") == false){
        alert("Enter a name that ends with .gpx");
    }
    else{

        $.ajax({
            type: "get",
            dataType: "json",
            url: "/createGPXEndpoint",
            data: {
                fileName: docName,
                creator: docCreator,
                version: docVersion
            },
            success: function(){
                console.log("success! " + data.createGPX);
                alert("Successfully added new gpx: " + docName);
            },
            fail: function(error){
                console.log("failed to create");
                console.log(error);
                alert("Error: failed to create.");
            }
            // Create an object for connecting to another waypoint
        });

    }
}

function createAddRouteForm(){
    numWpt = 0;
    $('#waypointCountTracker').html("");
    waypoints = [];
}

// fix this for query button
function dropdownButton(file){
    console.log(file.innerHTML);

    $("#compTable").remove();

    $("#renameForm").html("<p>Please choose a component</p><div id=\"renameButtonDiv\"><h4>No File Selected</h4></div>");


    var componentTableString = "<table id=\"compTable\" class=\"table table-bordered table-hover table-curved\"><thead class=\"thead-light\"><tr><th scope=\"col\">Component</th><th scope=\"col\">Name</th><th scope=\"col\">Number of Points</th><th scope=\"col\">Length</th><th scope=\"col\">Loop</th></tr></thead>";
    var routeFormString = "<form ref=\"addRouteForm\" id=\"addRouteForm\">    <div class=\"form-group\">        <div id=\"addRouteFileSelected\">            <h5>No File Selected</h5>        </div><div id=\"waypointCountTracker\">     </div>   <label for=\"addRouteInput\">   Enter Route Name  </label>   <input type=\"text\" class=\"form-control\" id=\"addRouteNameInput\" placeholder=\"Route Name\">   <p> <hr>  <label for=\"addRouteLat\">Enter Latitude  </label>        <input type=\"text\" class=\"form-control\" id=\"addRouteLat\" placeholder=\"Latitude\"><p> <label for=\"addRouteLon\">Enter Longitude </label> <input type=\"text\" class=\"form-control\" id=\"addRouteLon\" placeholder=\"Longitude\"> <p> </div>  <form> <div id=\"addRouteDiv\"></div> <div class=\"form-group\">  <input type='submit' class=\"btn btn-primary\" value=\"Add Waypoint\">  </div>";
    var otherDataButtonString = "";
    var renameButtonString = "";

    arrayOfCompObj = [];

    $.ajax({
        type: "get",
        dataType: "json",
        url: "/componentQuery",
        data: {
            fileName: file.innerHTML
        },
        success: function (gpxData) {

            componentFileName = file.innerHTML;

            
            gpxData.forEach(function(data) {
                // console.log("data name:" + data.name);
                componentTableString+="<tbody>";
                if (data.fileName === file.innerHTML){
                    // console.log("file: "+data.fileName);

                    arrayOfCompObj.push(data);
                    componentFileName = data.fileName;
                    
                    
                    componentTableString+=("<tr>");

                    componentTableString+=("<th scope=\"col\">" +data.compName+ "</th>");
                    componentTableString+=("<th scope=\"col\">" + data.name+ "</th>");
                    componentTableString+=("<th scope=\"col\">" +data.numPoints+ "</th>");
                    componentTableString+=("<th scope=\"col\">" +data.len+ "</th>");
                    componentTableString+=("<th scope=\"col\">" +data.loop+ "</th>");
                    
                    componentTableString+=("</tr>");

                    otherDataButtonString+="<button type=\"button\" onclick=\"dataButton(this)\" id=\"ComponentButton\" class=\"btn btn-primary Button\" data-dismiss=\"modal\">"+data.compName+"</button>";
                    renameButtonString+="<button type=\"button\" onclick=\"renameButton(this)\" id=\"ComponentButton\" class=\"btn btn-primary Button\" data-dismiss=\"modal\">"+data.compName+"</button>";
                }

            });

            console.log("hello");

            componentTableString+="</tbody>";
            componentTableString+="</table>";
            $("#gpxViewPanel").html(componentTableString);
            $("#otherDataButtonsDiv").html(otherDataButtonString);
            $("#renameButtonDiv").html(renameButtonString);
            finalRenameButtons = renameButtonString;
            console.log(componentFileName);
            $("#compStatus").html("<p>viewing: "+componentFileName+"</p>");

            $("#addRouteForm").html(routeFormString);
            
            $("#addRouteFileSelected").html("<h5>Adding Route to: "+componentFileName+"</h5>");

        },
        fail: function (error) {
            alert("Error creating component table");
        }
       
    
    });
    
    
    
}

function createRenameButtons(){
    // console.log(renameButtonString);
    // $("#renameButtonDiv").html(finalRenameButtons);
}

function dataButton(file){
    console.log(file.innerHTML);
    
    // console.log(componentFileName);
    // console.log(componentData);

    console.log(arrayOfCompObj);

    let alertString = "Other Data\n";

    arrayOfCompObj.forEach(function(comp){
        if (comp.compName == file.innerHTML){
            
            comp.otherData.forEach(function(oData){
                alertString+="Name: " + oData.name + " Value: " + oData.value +"\n";
            })
            
        }
    })

    alert(alertString);

    // console.log(componentOtherData);
    //have to get other data from selected button

    // $.ajax({
    //     type: "get",
    //     dataType: "json",
    //     url: "/otherData",
    //     data: {
    //         fileName: componentFileName
    //     },
    //     success: function (data) {

    //         // console.log(data);

    //         data.forEach(function(comp){
    //             console.log(comp);
    //             // if (comp.compName == file.innerHTML){

    //             // }
    //         })
    //         // alert("Other Data \n name: " +data.name +"\n value: " + data.value);
            
    //     },
    //     fail: function (error) {
            
    //     }
    
    // })
    
}

function renameButton(file){
    // console.log(file.innerHTML);
    componentName = file.innerHTML;

    let formStr = "<form ref=\"renameForm\" id= \"renameForm\"><div class=\"form-group\"><label for=\"formGroupExampleInput\">Rename Component: "+file.innerHTML+"</label><input type=\"text\" class=\"form-control\" id=\"formGroupExampleInput\" placeholder=\"New Name\"> </div><form><div id=\"renameButtonDiv\"></div><div class=\"form-group\"><input type='submit' class=\"btn btn-secondary\"></div>";

    $("#renameForm").html(formStr);
    //create a form that can be filled

    
    
}

function cancelRename(){

    $("#renameForm").html("<p>Please choose a component</p><div id=\"renameButtonDiv\"><h4>No File Selected</h4></div>");
    
    $("#renameButtonDiv").html(finalRenameButtons);
    
}

function findPathButton(){

}