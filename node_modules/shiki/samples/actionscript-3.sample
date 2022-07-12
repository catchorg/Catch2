private function createParticles( count ):void{
    var anchorPoint = 0;
    for(var i:uint = 0; i < count; i++){
     
        var particle:Object;
        if( inactiveFireParticles.length > 0 ){ 
            particle = inactiveFireParticles.shift();
        }else {
            particle = new Object(); 
            fireParticles.push( particle );                 
        }               
         
        particle.x = uint( Math.random() * frame.width * 0.1 ) + anchors[anchorPoint];
        particle.y = frame.bottom;
        particle.life = 70 + uint( Math.random() * 30 ); 
        particle.size = 5 + uint( Math.random() * 10 );
         
        if(particle.size > 12){
            particle.size = 10;
        }
        particle.anchor = anchors[anchorPoint] + uint( Math.random() * 5 );
         
        anchorPoint = (anchorPoint == 9)? 0 : anchorPoint + 1;
    }       
}

// From https://code.tutsplus.com/tutorials/actionscript-30-optimization-a-practical-example--active-11295