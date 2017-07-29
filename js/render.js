d3.json("./single_genre.json", function(error, data) {
    nv.addGraph(function() {
        var chart = nv.models.scatterChart()
            .showDistX(true)
            .showDistY(true)
            .color(d3.scale.category10().range());

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

        // chart.scatter.onlyCircles(false);

        chart.scatter.dispatch.on("elementClick", function(d) {
            // console.log( d.point.word.split("_")[0] );
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
});