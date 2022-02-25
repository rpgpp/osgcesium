function callFromC(func){
  if(Cesium.defined(func)){
    try{
      return func();
    }
    catch(err){
      alert(err);
    }
  }
}

window.initWe = {
  showNavigationWidget: true,
  showDistanceLengthWidget: true,
  debugShowFramesPerSecond: true,
  animation: true,
  shouldAnimate: true,
  timeline: true,
}

function clearAll() {
    const { WE } = window;
    WE.layerManager.remove("Radar");
    WE.layerManager.remove("3dtiles");
}

function showOrHideAnno() {
    var layername = "×¢¼Ç";
    var layer = WE.layerManager.getById(layername);
    if (!Cesium.defined(layer)) {
        WE.layerManager.addImageryProvider(WeatherEarth.CommonLayers.TDT_Cia, layername);
    }
    WE.layerManager.raiseToTop(layername);
    layer.show = !layer.show;
}

function init3d() {
    WeatherEarth.Config.WeatherDataTemplateUrl = '../../OCserver/p/OC?method=nc&param={0}%3B{1}%3B{2}%3B{3}';
    WeatherEarth.Config.WeatherDataVersion = '2.0';
}

var remoteUrlPost = "../../OCserver/weathertoy?m=";

function loadRadar(json) {
    WeatherEarth.Config.CreateTransferTextureFunction = undefined;
    const { WE } = window;

    var layername = "Radar";
    var layer = WE.layerManager.getById(layername);
    if (Cesium.defined(layer)) {
        WE.layerManager.remove(layername);
    }
    var options = JSON.parse(json);

    var nc = {
        name: options.name,
        varname: options.varname,
        ext: options.ext,
    };

    var volumeOptions = Object.assign(nc, {
        url: remoteUrlPost,
        TransFunction: WeatherEarth.WeatherVolumeTransfunctions.Transfunction_CommonTF,
        ValueAndColorRamp: WeatherEarth.WeatherVolumeTransfunctions.ValueAndColorRamp_DBZ,
        ffx: 4,
        postData: options.filename,
    });
    layer = WE.layerManager.addVolumeLayer(volumeOptions, layername);
    layer.showInner = false;
    layer.alpha = 1.0;
    layer.readyPromise.then(function () {
    });

    WE.layerManager.raiseToTop("×¢¼Ç");

    showMessageBox('Loading ' + options.filename);
}

function loadRadar2(json) {
    const { WE } = window;

    var layername = "Radar";
    var layer = WE.layerManager.getById(layername);
    if (Cesium.defined(layer)) {
        WE.layerManager.remove(layername);
    }
    var options = JSON.parse(json);

    var nc = {
        name: options.name,
        varname: options.varname,
        ext: options.ext,
    };

    WeatherEarth.Config.WeatherDataTemplateUrl = '../../OCserver/p/OC?method=nc&param={0}%3B{1}%3B{2}%3B{3}';
    WeatherEarth.Config.WeatherDataVersion = '2.0';

    var remoteUrlPost = "../../OCserver/b/OG?method=volume&param={0}%3B{1}%3B{2}%3B{3}";

    var volumeOptions = Object.assign(nc, {
        url: remoteUrlPost,
        TransFunction: WeatherEarth.WeatherVolumeTransfunctions.Transfunction_CommonTF,
        ValueAndColorRamp: WeatherEarth.WeatherVolumeTransfunctions.ValueAndColorRamp_DBZ,
        ffx: 4,
        postData: options.filename,
    });
    layer = WE.layerManager.addVolumeLayer(volumeOptions, layername);
    layer.showInner = false;
    layer.alpha = 1.0;
    layer.readyPromise.then(function () {
    });

    WE.layerManager.raiseToTop("×¢¼Ç");

    showMessageBox('Loading ' + options.filename);
}
function loadPNG(filename) {
    var rectangle = Cesium.Rectangle.fromDegrees(-180.0,-90,180.0,90);
    var geometry = new Cesium.RectangleGeometry({
        rectangle,
    });
    var geometryInstances = [];
    geometryInstances.push(
        new Cesium.GeometryInstance({
            geometry,
        })
    );
    var options = {
        geometryInstances: geometryInstances,
        appearance: new Cesium.MaterialAppearance({
            renderState: {
                depthTest: { enabled: true },
                depthMask: true,
                blending: Cesium.BlendingState.ALPHA_BLEND,
                cull: {
                    enabled: false,
                    face: Cesium.CullFace.FRONT,
                },
            },
            translucent: false,
        }),
        asynchronous: true,
    };

    const image = filename;

    var primitive = new Cesium.GroundPrimitive(options);
    primitive.appearance.material = new Cesium.Material({
        fabric: {
            type: "Image",
            uniforms: {
                image,
            },
        },
    });

    WE.rectangle = WE.viewer.scene.primitives.add(primitive);
    showMessageBox('Loading ' + filename);
}

function load3DTiles(filename) {
    const { WE } = window;

    var layername = "3dtiles";
    var layer = WE.layerManager.getById(layername);
    if (Cesium.defined(layer)) {
        WE.layerManager.remove(layername);
    }

    layer = WE.layerManager.addTilesetLayer({
        url: filename,
        debugShowBoundingVolume: true,
    },'3dtiles');

    layer.readyPromise.then(function (tileset) {
        WE.viewer.zoomTo(tileset, new Cesium.HeadingPitchRange(0.0, -0.8, tileset.boundingSphere.radius * 1.3));
    }).otherwise(function (error) {
        console.log(error);
    });

    showMessageBox('Loading ' + filename);
}

function enablePick(){
    const { WE } = window;

    WE.handlerManager.startPick({
        handleType: 'CommonPick',
        color: Cesium.Color.RED,
    });
}

function loadKML(host, filename) {
    const { WE } = window;
    var options = {
        camera: WE.viewer.scene.camera,
        canvas: WE.viewer.scene.canvas,
    };

    WE.viewer.dataSources.add(Cesium.KmlDataSource.load(filename, options));
}

new QWebChannel(qt.webChannelTransport, function (channel) {
    window.bridge = channel.objects.Bridge;
});

function ceval(method,json) {
    if (window.bridge) {
        bridge.eval(method, json)
    } else {
        alert('failed');
    }
}

function showMessageBox(message) {
    app.__vue__.$message({
        message: message,
        offset: 50
    })
}

function showColorCard(show) {
    app.__vue__.$store.commit('showColorCard', show)
}
