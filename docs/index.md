# Kitupiikki <small>Avoimen lähdekoodin kirjanpitäjä</small>

<div class="karuselli">
<div class="karusellissa fade">
 <img src="images/kitupiikkikannettava.png">
  <div class="txt">Avoimen lähdekoodin kirjanpitäjä</div>
</div>
 <div class="karusellissa fade">
  <img src="kirjaus/myllykirjaus.png">
  <div class="txt">Kitupiikki tallentaa pdf-muotoiset liitteet</div>
 </div>
 <div class="karusellissa fade">
  <img src="images/paakirja.png">
  <div class="txt">Pääkirja</div>
 </div>
 <div class="karusellissa fade">
  <img src="laskutus/luettelo.png">
  <div class="txt">Kitupiikkiin sisältyy myös laskutus</div>
 </div>
 <div class="karusellissa fade">
  <img src="images/maksualv.png">
  <div class="txt">Kitupiikki tukee maksuperusteista arvonlisäveroa</div>
 </div>
 <div class="karusellissa fade">
  <img src="images/arkisto.png">
  <div class="txt">Kirjanpidosta muodostetaan sähköinen arkisto</div>
 </div>
 <div class="karusellissa fade">
  <img src="images/raportinmuokkaus.png">
  <div class="txt">Tulosteet ovat täysin muokattavissa</div>
 </div>
 <div class="karusellissa fade">
  <img src="aloitus/vinkit7.png">
  <div class="txt">Kitupiikki vinkkaa ja neuvoo</div>
 </div>
 <div class="karusellissa fade">
  <img src="images/viivakoodi.png">
  <div class="txt">Laskuissa pankkiviivakoodi ja QR-koodi</div>
 </div>
 <div class="karusellissa fade">
  <img src="images/arkisto1.png">
  <div class="txt">Sähköisen arkiston etusivu</div>
 </div>
 <div class="karusellissa fade">
  <img src="maaritykset/kohdennukset/kohdennukset.png">
  <div class="txt">Kirjauksia voi kohdentaa toiminnoille, projekteille jne.</div>
 </div>
 <div class="karusellissa fade">
   <img src="images/poisto.png">
   <div class="txt">Kitupiikki laskee tasaerä- ja menojäännöspoistot.</div>
 </div>
</div>


Kitupiikki on ilmainen suomalainen kirjanpito-ohjelma. Kitupiikki käsittelee ja arkistoi skannatut tositteet. Ohjelma on alunperin suunniteltu yhdistyksille, mutta Kitupiikin avulla hoidat vaivattomasti myös pienehkön yrityksen kirjanpidon.

<div class="asennusinfo">
<h3>Kitupiikki 0.11 (beta) julkaistu 22.4.2018</h3>
Uutena hankekohdennus, pdf-raporttien sisäänrakennettu esikatselu, tositteen kopiointi uuden pohjaksi, tositteen tulostus ja laskujen tulostus ikkunakuoriin sopiviksi.

<div class="asennuslaatikko">
<a href="asennus" style="color:white;"><span class="fa fa-download"></span> Lataa Kitupiikki <span class="fa fa-windows"></span> <span class="fa fa-linux"></span></a>
</div>

</div>

<script>
var slideIndex = Math.floor( Math.random() * document.getElementsByClassName("karusellissa").length );
showSlides();

function showSlides() {
    var i;
    var slides = document.getElementsByClassName("karusellissa");
    for (i = 0; i < slides.length; i++) {
        slides[i].style.display = "none";
    }

    slideIndex = slideIndex + 1;
    if( slideIndex >= slides.length)
      slideIndex = 0;

    slides[ slideIndex ].style.display = "block";
    setTimeout(showSlides, 4000); // Kuva vaihtuu kahden sekunnin välein
}

</script>