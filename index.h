const char MAIN_page[] PROGMEM = R"=====(

<!DOCTYPE html>
<html lang="pt-BR">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Automation using IOT</title>
    <style>
        .tudo {
            display: table;
        }

        .menu {
            float: left;
            width: 30%;
        }

        .conteudo {
            margin-left: 35%;
        }


        .switch {
            position: relative;
            display: inline-block;
            width: 60px;
            height: 34px;
        }

        .switch input {
            opacity: 0;
            width: 0;
            height: 0;
        }

        .slider {
            position: absolute;
            cursor: pointer;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background-color: #ccc;
            -webkit-transition: .4s;
            transition: .4s;

        }

        .slider:before {
            position: absolute;
            content: "";
            height: 26px;
            width: 26px;
            left: 4px;
            bottom: 4px;
            background-color: white;
            -webkit-transition: .4s;
            transition: .4s;
        }

        input:checked+.slider {
            background-color: #2196F3;
        }

        input:focus+.slider {
            box-shadow: 0 0 1px #2196F3;
        }

        input:checked+.slider:before {
            -webkit-transform: translateX(26px);
            -ms-transform: translateX(26px);
            transform: translateX(26px);
        }

        /* Rounded sliders */
        .slider.round {
            border-radius: 34px;
        }

        .slider.round:before {
            border-radius: 50%;
        }


        .container {
            display: grid;
            grid-template-columns: auto auto;
            /* 2 COL LAYOUT */
        }

        /*FIM DO SWITCH*/


        div>h1 {
            text-align: center;
            font-size: 230%;
            margin-top: 0px;
            padding-top: 10px;
            width: auto;
            height: 0px;

        }

        div>h2 {
            padding-top: 2%;
            text-align: center;
        }

        div {
            background-color: rgb(145, 201, 201);
        }

        div>img {
            margin-left: auto;
            margin-right: auto;
            display: flex;
            justify-content: center;
            align-items: center;
        }

        .divL,
        .divR {
            box-sizing: border-box;
            padding: 5px;
            /*adicionadas por mim*/
            border: 2px solid #ccc;


        }

        .divL .divR,
        button,
        p {
            padding: 10px 20px 10px 20px;
            /*espaçamento interno dos botoẽs*/
            text-transform: capitalize;
            text-shadow: 2px 3px #ccc;
            margin-left: auto;
            margin-right: auto;
            display: flex;
            justify-content: center;
            align-items: center;

        }

        .divL .divR,
        .switch {

            text-transform: capitalize;
            text-shadow: 2px 3px #ccc;
            margin-left: auto;
            margin-right: auto;
            display: flex;
            justify-content: center;
            align-items: center;

        }

        .divL {
            background: #ffffff;
            /*OPTIONAL WIDTH
    width: 40%; */

        }

        .divR {
            background: #ffffff;
            /* OPTIONAL WIDTH 
width: 60% */
        }

        .dot {
            height: 40px;
            width: 20px;
            background-color: red;
            border-radius: 50%;
            margin-left: auto;
            margin-right: auto;
            display: flex;
            justify-content: center;
            border: 4px solid #e3ecec;



        }

        @media only screen and (max-width: 768px) {

            .divL,
            .divR {
                float: none;
                display: block;
                width: 100%;
            }
        }
    </style>

    <script>
        (function () {

            document.addEventListener('DOMContentLoaded', function () {
                document.addEventListener('DOMContentLoaded', receiveState());

                const returnstate = "/returnstate"
                const url = "/writeatuador";
                var returnStates;


                //SLIDE BUTTON
                var checkClassSwitch = document.querySelectorAll('.switch input[type=checkbox]')

                checkClassSwitch.forEach(function (check) {
                    check.addEventListener('click', check_switchIndex);
                })
                var changeStatus;

                function check_switchIndex(event) {

                    //retorna status do objeto button
                    let swIndex = Array.from(checkClassSwitch).indexOf(event.target)
                    let status = checkClassSwitch[swIndex].checked
                    let id = checkClassSwitch[swIndex].id;
                    changeStatus = checkClassSwitch[swIndex].parentNode.parentNode.childNodes[3]
                    console.log(changeStatus.previousSibling.previousSibling);

                    sendJSON(varConcat(id, status));

                }

                //FIM SLIDE BUTTON


                function varConcat(pos, Status) {
                    let obj = {
                        id: pos,
                        status: Status
                    }
                    return obj
                }

                var sucess = "";

                function sendJSON(json) {

                    // Creating a XHR object
                    let xhr = new XMLHttpRequest();

                    // open a connection
                    xhr.open("POST", url, true);
                    xhr.withCredentials = true;
                    // Set the request header i.e. which type of content you are sending
                    xhr.setRequestHeader("Content-Type", "application/json");


                    // Create a state change callback
                    xhr.onreadystatechange = function () {
                        if (xhr.readyState === 4 && xhr.status === 200) {
                            // Print received data from server
                            sucess = parseInt(this.responseText);
                            console.log('return Endpoint: ' + sucess);

                            if (sucess == 1) {
                                changeStatus.textContent = "Ligado"
                                status = "true"
                                changeStatus.previousSibling.previousSibling.style.backgroundColor =
                                    "green"
                            }
                            if (sucess == 0) {
                                changeStatus.textContent = "Desligado";
                                status = "false"
                                changeStatus.previousSibling.previousSibling.style.backgroundColor =
                                    "red";
                            } else {

                            }

                        } else {
                            console.log("Mensagem não enviada");
                            changeStatus.textContent == "Ligado" ? status = "true" : status = "false";
                            sucess = "3"
                        }

                    };

                    // Converting JSON data to string
                    var data = JSON.stringify(json);
                    console.log(data);
                    //console.log(sucess);
                    // Sending data with the request
                    xhr.send(data);
                    return sucess;
                }

                function receiveState() {

                    var xhttp = new XMLHttpRequest();
                    xhttp.onreadystatechange = function () {
                        if (this.readyState == 4 && this.status == 200) {
                            document.getElementById("demo").innerHTML =
                                adjustInitiState(this.responseText);


                        }
                    };
                    xhttp.open("GET", "/returnstate", true);
                    xhttp.send();

                }

                function adjustInitiState(result) {
                    /*{"actuators":[{"id":"OUT1","status":false},{"id":"OUT2","status":false},
                    {"id":"OUT3","status":false},{"id":"OUT4","status":false}]} */
                    var json = JSON.parse(result);

                    console.log(json.actuators[0].id);

                    for (let index = 0; index < checkClassSwitch.length; index++) {

                        let jsonStatus = json.actuators[index];
                        let changeOnOffLabel = checkClassSwitch[index].parentNode.parentNode.childNodes[3];
                        let changeRedGreenBackground = changeOnOffLabel.previousElementSibling

                        if (jsonStatus.status) {
                            changeOnOffLabel.textContent = "Ligado";
                            checkClassSwitch[index].checked = "true";
                            changeRedGreenBackground.style.backgroundColor = "green";
                        } else {
                            changeOnOffLabel.textContent = "Desligado";
                            changeRedGreenBackground.style.backgroundColor = "red";

                        }
                    }

                }
            })

        })()
    </script>
</head>

<body>

    <div>
        <h1>AUTOMATION IOT </h1>
        <h2>with JS - CSS - HTML and Esp8266</h2>
    </div>

    <div class="container">

        <div class="divR">
            <p class="dot"></p>
            <p class="status">Indefinido</p>
            <label class="switch">
                <input id="OUT1" type="checkbox">
                <span class="slider"></span>
            </label>
        </div>

        <div class="divL">
            <p class="dot"></p>
            <p class="status">Indefinido</p>
            <label class="switch">
                <input id="OUT2" type="checkbox">
                <span class="slider"></span>
            </label>
        </div>

        <div class="divR">
            <p class="dot"></p>
            <p class="status">Indefinido</p>
            <label class="switch">
                <input id="OUT3" type="checkbox">
                <span class="slider"></span>
            </label>
        </div>

        <div class="divL">
            <p class="dot"></p>
            <p class="status">Indefinido</p>
            <label class="switch">
                <input id="OUT4" type="checkbox">
                <span class="slider"></span>
            </label>
        </div>


    </div>


</body>

</html>

)=====";