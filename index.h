const char MAIN_page[] PROGMEM = R"=====(

    <!DOCTYPE html>
<html>


<head>
    
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js"></script>
    <style >
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

        input:checked + .slider {
          background-color: #2196F3;
        }

        input:focus + .slider {
          box-shadow: 0 0 1px #2196F3;
        }

        input:checked + .slider:before {
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

    </style>

    <script>
      (function () {

$(document).ready(function () {

    document.querySelector("#submitDados").addEventListener("click", e => {
        var nome = document.querySelector("#prename").value
        var idade = parseInt(document.querySelector("#idade").value)
        alert(nome + idade)
    })

    //SLIDE BUTTON
    $('.switch').change(function () {

        var b = $(this).children("input").prop("checked")
        //var b1= document.getElementsByTagName("input").prop("checked")
        var a = $("input[type=checkbox]")
        //console.clear()
        console.log(b)
        if (b) {
            console.log("Check box in Checked")

        } else {
            //document.getElementsByClassName(a).checked = true;
            console.log("Check box in Unchecked")

        }
    })
    //FIM SLIDE BUTTON

    $("p").click(function () {
        $(this).css("color", "#0000ff")

    })


    $(".btMotor").click(function () {

        //retorna status do objeto button
        var statusValueLocal = $(this).attr('value')

        //retorna o indice do botão que foi acionado
        var objectPosition = $(this).index("button")
        //document.getElementById(".btMotor").innerHTML = "myJSON"
        //console.log(event.currentTarget)

        if (statusValueLocal == 1) {
            $(this).attr('value', 0)
            var statusValueChanged = 0
            var index = $("div").index();
        } else {
            $(this).attr('value', 1)
            var statusValueChanged = 1
        }

        //console.log("Posição do objeto:" + objectPosition + "\n Valor do status Anterior:" + statusValueLocal + "\nValor do status Modificado:" + statusValueChanged)
        var json = varConcat(objectPosition, statusValueChanged)
        //console.log(json)
        sendJSON(json)


    })

    function varConcat(pos, Status) {
        var obj = {id:pos,status:Status}
        return obj

    }

    function sendPost(varConcat) {

        var data = JSON.stringify(varConcat);
        console.log(data)
        var settings = {
            "url": "192.168.1.154/post",
            "withCredentials": "false",
            "method": "POST",
            "timeout": 0,
            "headers": {
            "Content-Type": ["application/json", "text/plain"]
            },
            "data": JSON.stringify(varConcat),
        };

        $.ajax(settings).done(function (response) {
            console.log(response);
        });



    }

    function sendJSON(json){


        // Creating a XHR object
        let xhr = new XMLHttpRequest();
        let url = "/writeatuador"

        // open a connection
        xhr.open("POST", url, true);
        xhr.withCredentials = false;
        // Set the request header i.e. which type of content you are sending
        xhr.setRequestHeader("Content-Type", "application/json");

        // Create a state change callback
        xhr.onreadystatechange = function () {
            if (xhr.readyState === 4 && xhr.status === 200) {

                // Print received data from server
                console.log(this.responseText);

            }
        };

        // Converting JSON data to string
        var data = JSON.stringify(json);
        console.log(data);

        // Sending data with the request
        xhr.send(data);
    }



})

})()
    </script>

</head>

<body>

    <div>
        <p class="status">Ligado</p>
        <img src="img_pulpitrock.jpg" alt="Pulpit Rock" width="284" height="213"><br>
        <button class="btMotor" value=1>Motor 1</button>
    </div>Digite seu nome

    <div>
        <p id="status2">Ligado</p>
        <img src="img_pulpitrock.jpg" alt="Pulpit Rock" width="284" height="213"><br>
        <button class="btMotor" value=1>Motor 2</button>
    </div>

    <div>
        <h2>Dados pessoais</h2>
        <input type="text" id="prename" autocomplete="false" placeholder="Digite seu nome"><br>

        <input type="text" id="idade" autocomplete="false" placeholder="Digite sua idade"> <br>
        <button id="submitDados" type="submit">Envie</button>

    </div>

    <h2>Toggle Switch</h2>

    <label class="switch">
        <input type="checkbox">
        <span class="slider"></span>
    </label>

    <label class="switch">
        <input type="checkbox">
        <span class="slider round"></span>
    </label>

    <label class="switch">
        <input type="checkbox">
        <span class="slider round"></span>
    </label>
</body>

</html>



)=====";