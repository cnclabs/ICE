$(document).ready(function() {
    $('input.autocomplete').autocomplete({
    data: autoTitle,
    limit: 10,
    onAutocomplete: function(val) {
        search(val);
    },
    minLength: 1
  });
 });
