    window.onload = init();
    window.setInterval(() => {
        init();
      }, 3* 1000);
    function init() {
        fetch('http://192.168.43.209/home', {
        method: 'get'
    }).then(res => res.json())//response type
    .then(function(data){
        display(data.temperature,"temp"),
        display(data.sensorValue,"lum");
        display(data.led,"stateled");
    });
    
    
    } 

    function display(donnee,id){
        document.getElementById(id).innerHTML = donnee;
        console.log(donnee);
    }
    

    function allumerled(){
        fetch('http://192.168.43.209/allume', 
        ).then(res => res.json())//response type
        .then(data => console.log(data.led));
   }

   function eteindreled(){
    fetch('http://192.168.43.209/eteindre', 
    ).then(res => res.json())//response type
    .then(data => console.log(data));
}
    