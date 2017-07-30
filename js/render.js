var all_data;

d3.json("./single_genre.json", function(error, data) {
    // console.log(data);
    all_data = data;
    render(all_data);
});

function render(data) {
    nv.addGraph(function() {
        var chart = nv.models.scatterChart()
            // .color(d3.scale.category10().range());

        chart.tooltip.contentGenerator(
            function(d) {
                var graph = "<div style='padding-left:5px; padding-top:2px; padding-right:5px; padding-bottom: 2px'><div style='display: inline-block; width: 10px; height: 10px; -moz-border-radius: 50px; -webkit-border-radius: 50px; border-radius: 50px; background:" + d.point.color + ";''></div>";
                var group_name = " " + d.series[0].key;
                var word_name = d.point.title;
                if (d.point.src) {
                    return graph + group_name + "<br><b style='align: center; padding-left:13px;' >" + word_name + "</b><br><img src='" + d.point.src + "'></div>";
                } else {
                    return graph + group_name + "<br><b style='align: center; padding-left:13px;' >" + word_name + "</b></div>";
                }
            });

        chart.xAxis.tickFormat(d3.format('.02f'));
        chart.yAxis.tickFormat(d3.format('.02f'));

        chart.scatter.dispatch.on("elementClick", function(d) {
            if (d.point.cate === "item") {
                window.open("http://www.imdb.com/title/" + d.point.imdb_id + "/", "_blank");
            } else {
                window.open("http://conceptnet.io/c/en/" + d.point.title);
            }
        });

        d3.select('#scatter svg')
            .datum(data)
            .transition().duration(500)
            .call(chart);

        nv.utils.windowResize(chart.update);

        return chart;
    });
}

function search(val) {
    var data = JSON.parse(JSON.stringify(all_data));
    data.forEach(function(cate) {
        cate.values.forEach(function(point, index) {
            if ($.inArray(point.title, neighbors[val]) === -1) {
                cate.values[index] = '';
            }
            if (point.title === val) {
                point.size = 1.0;
            }
        });
    });
    data.forEach(function(cate) {
        cate.values = cate.values.filter((x) => { return x !== '' })
    });
    render(data);
}

function searchEnter(e) {
    if (e.keyCode == 13) {
        var val = document.getElementById("autocomplete-input");
        if ($.inArray(val.value, Object.keys(neighbors))) {
            search(val.value);
        }
        return false;
    }
}

function reset() {
    render(all_data);
}