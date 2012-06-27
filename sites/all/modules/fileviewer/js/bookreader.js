(function ($) {
Drupal.behaviors.fileviewer = {
    attach: function (context) { 
        br = new BookReader();
        br.searchResults = null;
        br.server = Drupal.settings.fileviewer.server;
        br.bookId = Drupal.settings.fileviewer.bookId;
        br.bookPath = "meta";
        br.subPrefix = "text.xml";
        br.leafNumToIndex = function(leafNum) {
            return leafNum - 1;
        }
        // using a different URL structure
        br.getPageURI = function(index, reduce, rotate) {
            // reduce and rotate are ignored in this simple implementation, but we
            // could e.g. look at reduce and load images from a different directory
            // or pass the information to an image server
            var leafStr = this.numLeafs + '';
            var url = Drupal.settings.fileviewer.pdfFolder + '/page-'+ addZero(index + 1 + '', leafStr.length) + '.png';
            return url;
        }
        // Return which side, left or right, that a given page should be displayed on
        br.getPageSide = function(index) {
            if (0 == (index & 0x1)) {
                return 'R';
            } else {
                return 'L';
            }
        }
        br.getPageWidth = function(index) {
            if (index == 0) {index = 1;}
            if (null == br.textXML) {
                var newImg = new Image();
                return Drupal.settings.fileviewer.pageWidth;
            }
            else {
                return parseFloat(br.textXML.find("page[number=" + index + "]").attr("width"));
            }
        }
        br.getPageHeight = function(index) {
          if (index == 0) {index = 1;}
            if (null == br.textXML) {
                return Drupal.settings.fileviewer.pageHeight;
            }
          else {
                return parseFloat(br.textXML.find("page[number=" + index + "]").attr("height"));
            }
        }
        // This function returns the left and right indices for the user-visible
        // spread that contains the given index.  The return values may be
        // null if there is no facing page or the index is invalid.
        br.getSpreadIndices = function(pindex) {   
            var spreadIndices = [null, null]; 
            if ('rl' == this.pageProgression) {
                // Right to Left
                if (this.getPageSide(pindex) == 'R') {
                    spreadIndices[1] = pindex;
                    spreadIndices[0] = pindex + 1;
                } else {
                    // Given index was LHS
                    spreadIndices[0] = pindex;
                    spreadIndices[1] = pindex - 1;
                }
            } else {
                // Left to right
                if (this.getPageSide(pindex) == 'L') {
                    spreadIndices[0] = pindex;
                    spreadIndices[1] = pindex + 1;
                } else {
                    // Given index was RHS
                    spreadIndices[1] = pindex;
                    spreadIndices[0] = pindex - 1;
                }
            }
            
            return spreadIndices;
        }
// For a given "accessible page index" return the page number in the book.
//
// For example, index 5 might correspond to "Page 1" if there is front matter such
// as a title page and table of contents.
        br.getPageNum = function(index) {
            return index+1;
        }

// Total number of leafs
        br.numLeafs = Drupal.settings.fileviewer.pageNumber;

// Book title and the URL used for the book title link
        br.bookTitle = Drupal.settings.fileviewer.filename;
        br.bookUrl  = Drupal.settings.fileviewer.filepath;

        // Override the path used to find UI images
        br.imagesBaseURL = Drupal.settings.fileviewer.imagesBaseURL;

        br.getEmbedCode = function(frameWidth, frameHeight, viewParams) {
            return "Embed code not supported in Drupal.";
        }
        br.searchTerm = function(term, line){
            var left = line.search(term);
            var right = line.length - left - term.length;
            this.lRate = parseFloat(left/line.length);
            this.rRate = parseFloat(right/line.length);
            return parseFloat(left/line.length);
        }
        br.search = function (term) {
          if (term == "") return;
          this.searchTerm = term;
          this.removeSearchResults();
          var url = Drupal.settings.fileviewer.pdfFolder + "/meta/text.xml";
          this.showProgressPopup('<img id="searchmarker" src="'+this.imagesBaseURL + 'marker_srch-on.png'+'"> Search results will appear below...');

            br.XsearchResults = br.textXML.find("text:contains(" + term + ")");
            if (br.XsearchResults.length == 0) {
                br.searchResults = null;
                this.removeProgressPopup();
                return;
            }
            br.searchResults = {};
            br.XsearchResults.each(function(index) {
            if (br.searchResults.matches == undefined) {
              br.searchResults = {
                matches:[
                {
                text: $(this).text(),
                page: parseFloat($(this).parent().attr("number")), 
                page_width: parseFloat($(this).parent().attr("width")), 
                page_height: parseFloat($(this).parent().attr("height")),
                par: [{
                  boxes: [{
                      l: parseFloat($(this).attr("left")) + offsetLeft(term, $(this).text())*parseFloat($(this).attr("width")), 
                      r: (parseFloat($(this).attr("left")) + parseFloat($(this).attr("width"))) - offsetRight(term, $(this).text())*parseFloat($(this).attr("width")), 
                      t: parseFloat($(this).attr("top")), 
                      b: (parseFloat($(this).attr("top")) + parseFloat($(this).attr("height"))), 
                      page: parseFloat($(this).parent().attr("number"))
                  }]
                }]
              }
              ]
              };
            } 
            else {
            br.searchResults.matches.push( {
              par: [{
                text: $(this).text(),
                page: parseFloat($(this).parent().attr("number")), 
                page_width: parseFloat($(this).parent().attr("width")), 
                page_height: parseFloat($(this).parent().attr("height")),
                boxes: [{
                    l: parseFloat($(this).attr("left")) + offsetLeft(term, $(this).text())*parseFloat($(this).attr("width")), 
                    r: (parseFloat($(this).attr("left")) + parseFloat($(this).attr("width"))) - offsetRight(term, $(this).text())*parseFloat($(this).attr("width")), 
                      t: parseFloat($(this).attr("top")), 
                      b: (parseFloat($(this).attr("top")) + parseFloat($(this).attr("height"))), 
                      page: parseFloat($(this).parent().attr("number"))
                }]
              }]
            });
            }
              leafNum = parseInt($(this).parent().attr("number"));
              br.addSearchResult($(this).text(), br.leafNumToIndex(leafNum));
            });
            br.updateSearchHilites();
            br.removeProgressPopup();
        }
        // Let's go!
        br.ui = getURLParameter('ui');
        if (Drupal.settings.fileviewer.searchInside == true) {
            $.ajax({
                url: Drupal.settings.fileviewer.pdfFolder + "/meta/text.xml",
                dataType: "xml",
                success: function(data) {
                    br.textXML = $(data);
                    br.init();
                },
                error: function(){
                    br.init();
                    $('#BRtoolbar').find('#booksearch').hide();
                }
            });
        } else {
          br.init();
          $('#BRtoolbar').find('#booksearch').hide();
        }
        $('#BRtoolbar').find('.read').hide();
    }
};


})(jQuery);

function getURLParameter(name) {
    return unescape(
        (RegExp(name + '=' + '(.+?)(&|$)').exec(window.location.search)||[,null])[1]
    );
}

function addZero(s,n){
  while(s.length < n){
    s ='0' + s;
  }
  return(s);
}

function offsetLeft (term, line){
    var left = line.search(term);
    var right = line.length - left - term.length;
    return parseFloat(left/line.length);
}

function offsetRight (term, line){
    var left = line.search(term);
    var right = line.length - left - term.length;
    return parseFloat(right/line.length);
}
