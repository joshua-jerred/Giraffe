/* Collapsible -------------------------------------------------- */

// <div class="collapsible-box">
//  <div class="collapsible-title">Title</div>
//    <div class="collapsible-content" onclick>
//          Content
//    </div>
// </div>


window.addEventListener("DOMContentLoaded", function () {
  var coll = document.getElementsByClassName("collapsible");
  var i;
  
  for (i = 0; i < coll.length; i++) {
    coll[i].addEventListener("click", function() {
      this.classList.toggle("active");
      var content = this.nextElementSibling;
      if (content.style.maxHeight){
        content.style.maxHeight = null;
      } else {
        content.style.maxHeight = content.scrollHeight + "px";
      } 
    });
  }
});