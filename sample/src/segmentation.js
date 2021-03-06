var statusElement = document.getElementById('status');
var loadPhoto = document.getElementById('loadPhoto');
var saveButton = document.getElementById('save');
var restartButton = document.getElementById('restart');
var undoButton = document.getElementById('undo');
var redoButton = document.getElementById('redo');

var imageCanvas = document.getElementById('image');
var overlayCanvas = document.getElementById('overlay');

const FirstClick = 0, SecondClick = 1, TraceClicks = 2;
var segmentation, photoUtils, XDMUtils;
var imageContext, imageData, overlayContext;
var currentPhoto, savePhoto;

var width, height;
var curPhotoWidth, curPhotoHeight;
var topX, topY, bottomX, bottomY;
var lastX = -1, lastY = -1;
var nextClick = FirstClick;
var hasImage = false;
var stopDrawLine = false;
var markupImgHints = [];
var points = [];
var foreground;
var isLeftButtonDown = false, isRightButtonDown = false;

function getDateString() {
  var date = new Date();
  var dateString =
      date.getFullYear() +
      ('0' + (date.getMonth() + 1)).slice(-2) +
      ('0' + date.getDate()).slice(-2) +
      ('0' + date.getHours()).slice(-2) +
      ('0' + date.getMinutes()).slice(-2) +
      ('0' + date.getSeconds()).slice(-2);
  return dateString;
}

function doBlendColorPop(colorImage, maskImage) {
  var GREY = 0x7f;
  var alpha;
  for (var x = 0; x < colorImage.width; x++) {
    for (var y = 0; y < colorImage.height; y++) {
      var index = y * colorImage.width * 4 + x * 4;
      var maskIndex = y * maskImage.width + x;
      var mask = maskImage.data[maskIndex];
      if (mask > 0) alpha = 1.0;
      else alpha = 0.0;

      colorImage.data[index] = parseInt(alpha * colorImage.data[index] +
          (1 - alpha) * ((colorImage.data[index] >> 4) + GREY));
      colorImage.data[index + 1] = parseInt(alpha * colorImage.data[index + 1] +
          (1 - alpha) * ((colorImage.data[index + 1] >> 4) + GREY));
      colorImage.data[index + 2] = parseInt(alpha * colorImage.data[index + 2] +
          (1 - alpha) * ((colorImage.data[index + 2] >> 4) + GREY));
    }
  }
}

function resetMarkupImgHints() {
  if (markupImgHints.length) markupImgHints = [];
  var len = curPhotoWidth * curPhotoHeight;
  for (var i = 0; i < len; i++) {
    markupImgHints[i] = 0;
  }
}

function resetPoints() {
  if (points.length) points = [];
}

function createInitialMask() {
  var value = 255;
  for (var y = topY; y != bottomY; y += (topY < bottomY) ? 1 : -1)
  {
    for (var x = topX; x != bottomX; x += (topX < bottomX) ? 1 : -1)
    {
      markupImgHints[y * curPhotoWidth + x] = value;
    }
  }
}

function connectMissedPixels(starts, ends) {
  if (starts.x == ends.x)
  {
    for (var y = starts.y; y != ends.y; y += (starts.y < ends.y) ? 1 : -1)
    {
      points.push({x: starts.x, y: y});
    }
  } else {
    var slope = (ends.y - starts.y) / parseFloat(ends.x - starts.x);

    for (var x = starts.x; x != ends.x; x += (starts.x < ends.x) ? 1 : -1)
    {
      var y = parseInt((slope * (x - starts.x) + starts.y) + 0.5);
      points.push({x: x, y: y});
    }
  }
}

function updateMarkupImage(pointX, pointY, isForeground) {
  points.push({x: pointX, y: pointY});

  foreground = isForeground;

  if (lastX == -1 && lastY == -1)
    points.push({ x: pointX, y: pointY });
  else
    connectMissedPixels({x: lastX, y: lastY}, {x: pointX, y: pointY});

  lastX = pointX;
  lastY = pointY;
}

function main() {
  photoUtils = realsense.DepthEnabledPhotography.PhotoUtils;
  XDMUtils = realsense.DepthEnabledPhotography.XDMUtils;

  imageContext = imageCanvas.getContext('2d');
  overlayContext = overlay.getContext('2d');

  overlayCanvas.addEventListener('mousedown', function(e) {
    if (!hasImage)
      return;
    if (nextClick == FirstClick) {
      topX = parseInt((e.clientX - overlayCanvas.offsetLeft) * width / imageCanvas.scrollWidth);
      topY = parseInt((e.clientY - overlayCanvas.offsetTop) * height / imageCanvas.scrollHeight);
      nextClick = SecondClick;
    } else if (nextClick == SecondClick) {
      bottomX = parseInt((e.clientX - overlayCanvas.offsetLeft) * width / imageCanvas.scrollWidth);
      bottomY = parseInt((e.clientY - overlayCanvas.offsetTop) * height / imageCanvas.scrollHeight);

      createInitialMask();

      var markupImage = {
        format: 'Y8',
        width: curPhotoWidth,
        height: curPhotoHeight,
        data: markupImgHints
      };
      segmentation.objectSegment(currentPhoto, markupImage).then(
          function(maskImage) {
            currentPhoto.queryContainerImage().then(
                function(colorImage) {
                  doBlendColorPop(colorImage, maskImage);

                  imageContext.clearRect(0, 0, width, height);
                  imageData = imageContext.createImageData(
                      colorImage.width, colorImage.height);
                  imageData.data.set(colorImage.data);
                  imageContext.putImageData(imageData, 0, 0);

                  currentPhoto.clone().then(
                      function(photo) {
                        savePhoto = photo;
                        savePhoto.setContainerImage(colorImage).then(
                            function() {
                              statusElement.innerHTML = 'Finish processing segmentation!' +
                                  'Click and drag the left(foregroud) and right(background)' +
                                  'mouse buttons to refine the mask';
                              nextClick = TraceClicks;
                            },
                            function(e) { statusElement.innerHTML = e.message; });
                      },
                      function(e) { statusElement.innerHTML = e.message; });
                },
                function(e) { statusElement.innerHTML = e.message });
          },
          function(e) { statusElement.innerHTML = e.message });
    } else if (nextClick == TraceClicks) {
      stopDrawLine = false;
      resetPoints();
      var x = parseInt((e.clientX - overlayCanvas.offsetLeft) * width / imageCanvas.scrollWidth);
      var y = parseInt((e.clientY - overlayCanvas.offsetTop) * height / imageCanvas.scrollHeight);
      overlayContext.beginPath();
      if (e.button == 0) {
        isLeftButtonDown = true;
        updateMarkupImage(x, y, true);
        overlayContext.strokeStyle = 'blue';
      } else {
        isRightButtonDown = true;
        updateMarkupImage(x, y, false);
        overlayContext.strokeStyle = 'green';
      }
      overlayContext.lineWidth = 0.5;
      overlayContext.moveTo(x, y);
    }
  }, false);

  overlayCanvas.addEventListener('mousemove', function(e) {
    if (!hasImage || nextClick == FirstClick ||
        (nextClick == TraceClicks && !isLeftButtonDown && !isRightButtonDown))
      return;
    bottomX = parseInt((e.clientX - overlayCanvas.offsetLeft) * width / imageCanvas.scrollWidth);
    bottomY = parseInt((e.clientY - overlayCanvas.offsetTop) * height / imageCanvas.scrollHeight);
    if (nextClick == SecondClick) {
      overlayContext.clearRect(0, 0, width, height);
      var offsetX = bottomX - topX;
      var offsetY = bottomY - topY;
      overlayContext.strokeStyle = 'red';
      overlayContext.lineWidth = 2;
      overlayContext.strokeRect(topX, topY, offsetX, offsetY);
    } else if (nextClick == TraceClicks && (isLeftButtonDown || isRightButtonDown)) {
      if (!stopDrawLine) {
        overlayContext.lineTo(bottomX, bottomY);
        overlayContext.stroke();
        // for precessing
        if (isLeftButtonDown) {
          updateMarkupImage(bottomX, bottomY, true);
        } else if (isRightButtonDown) {
          updateMarkupImage(bottomX, bottomY, false);
        }
      } else {
        overlayContext.closePath();
      }
    }
  }, false);

  overlayCanvas.addEventListener('mouseup', function(e) {
    if (nextClick == TraceClicks) {
      if (e.button == 0) {
        isLeftButtonDown = false;
      } else {
        isRightButtonDown = false;
      }
      stopDrawLine = true;
      lastX = lastY = -1;

      segmentation.refineMask(points, foreground).then(
          function(maskImage) {
            currentPhoto.queryContainerImage().then(
                function(colorImage) {
                  doBlendColorPop(colorImage, maskImage);

                  imageContext.clearRect(0, 0, width, height);
                  imageData = imageContext.createImageData(
                      colorImage.width, colorImage.height);
                  imageData.data.set(colorImage.data);
                  imageContext.putImageData(imageData, 0, 0);

                  currentPhoto.clone().then(
                      function(photo) {
                        savePhoto = photo;
                        savePhoto.setContainerImage(colorImage).then(
                            function() {
                              statusElement.innerHTML = 'Click and drag the left(foregroud) and' +
                              'right(background) mouse buttons to refine the mask';
                            },
                            function(e) { statusElement.innerHTML = e.message; });
                      },
                      function(e) { statusElement.innerHTML = e.message; });
                },
                function(e) { statusElement.innerHTML = e.message; });
          },
          function(e) { statusElement.innerHTML = e.message; });
    }
  }, false);

  saveButton.onclick = function(e) {
    if (!savePhoto) {
      statusElement.innerHTML = 'There is no photo to save';
      return;
    }
    XDMUtils.saveXDM(savePhoto).then(
        function(blob) {
          xwalk.experimental.native_file_system.requestNativeFileSystem('pictures',
              function(fs) {
                var fileName = '/pictures/depthphoto_' + getDateString() + '.jpg';
                fs.root.getFile(fileName, { create: true }, function(entry) {
                  entry.createWriter(function(writer) {
                    writer.onwriteend = function(e) {
                      statusElement.innerHTML =
                          'The depth photo has been saved to ' + fileName + ' successfully.';
                    };
                    writer.onerror = function(e) {
                      statusElement.innerHTML = 'Failed to save depth photo.';
                    };
                    writer.write(blob);
                  },
                  function(e) { statusElement.innerHTML = e; });
                },
                function(e) { statusElement.innerHTML = e; });
              });
        },
        function(e) { statusElement.innerHTML = e.message; });
  };

  loadPhoto.addEventListener('change', function(e) {
    var file = loadPhoto.files[0];
    XDMUtils.isXDM(file).then(
        function(success) {
          if (success) {
            XDMUtils.loadXDM(file).then(
                function(photo) {
                  currentPhoto = photo;
                  savePhoto = photo;
                  if (!segmentation) {
                    try {
                      segmentation =
                          new realsense.DepthEnabledPhotography.Segmentation();
                    } catch (e) {
                      statusElement.innerHTML = e.message;
                      return;
                    }
                  }

                  currentPhoto.queryContainerImage().then(
                      function(image) {
                        curPhotoWidth = image.width;
                        curPhotoHeight = image.height;
                        width = image.width;
                        height = image.height;
                        imageCanvas.width = width;
                        imageCanvas.height = height;
                        overlayCanvas.width = width;
                        overlayCanvas.height = height;
                        resetMarkupImgHints();
                        resetPoints();
                        nextClick = FirstClick;
                        imageContext.clearRect(0, 0, width, height);
                        imageData = imageContext.createImageData(image.width, image.height);
                        statusElement.innerHTML = 'Select the bounding box around the object.';
                        overlayContext.clearRect(0, 0, width, height);
                        imageData.data.set(image.data);
                        imageContext.putImageData(imageData, 0, 0);
                        hasImage = true;
                        photoUtils.getDepthQuality(currentPhoto).then(
                            function(quality) {
                              statusElement.innerHTML += ' The photo quality is ' + quality;
                            },
                            function(e) { statusElement.innerHTML = e.message; });
                      },
                      function(e) { statusElement.innerHTML = e.message; });
                },
                function(e) { statusElement.innerHTML = e.message; });
          } else {
            statusElement.innerHTML = 'This is not a XDM file. Load failed.';
          }
        },
        function(e) { statusElement.innerHTML = e.message; });
  });

  restartButton.onclick = function(e) {
    if (!hasImage) {
      statusElement.innerHTML = 'Please load a photo.';
      return;
    }
    overlayContext.clearRect(0, 0, width, height);
    currentPhoto.queryContainerImage().then(
        function(image) {
          curPhotoWidth = image.width;
          curPhotoHeight = image.height;
          resetMarkupImgHints();
          resetPoints();
          nextClick = FirstClick;
          imageContext.clearRect(0, 0, width, height);
          imageData =
              imageContext.createImageData(image.width, image.height);
          statusElement.innerHTML = 'Select the bounding box around the object.';
          overlayContext.clearRect(0, 0, width, height);
          imageData.data.set(image.data);
          imageContext.putImageData(imageData, 0, 0);
          hasImage = true;
        },
        function(e) { statusElement.innerHTML = e.message; });
  };

  undoButton.onclick = function(e) {
    if (!segmentation || !hasImage) {
      statusElement.innerHTML = 'Undo Scribble Error';
      return;
    }
    segmentation.undo().then(
        function(maskImage) {
          currentPhoto.queryContainerImage().then(
              function(colorImage) {
                doBlendColorPop(colorImage, maskImage);

                imageContext.clearRect(0, 0, width, height);
                imageData = imageContext.createImageData(
                    colorImage.width, colorImage.height);
                imageData.data.set(colorImage.data);
                imageContext.putImageData(imageData, 0, 0);

                currentPhoto.clone().then(
                    function(photo) {
                      savePhoto = photo;
                      savePhoto.setContainerImage(colorImage).then(
                          function() {
                            statusElement.innerHTML = 'Undo Scribble Complete';
                          },
                          function(e) { statusElement.innerHTML = e.message; });
                    },
                    function(e) { statusElement.innerHTML = e.message; });
              },
              function(e) { statusElement.innerHTML = e.message; });
        },
        function(e) { statusElement.innerHTML = e.message; });
  };

  redoButton.onclick = function(e) {
    if (!segmentation || !hasImage) {
      statusElement.innerHTML = 'Redo Scribble Error';
      return;
    }
    segmentation.redo().then(
        function(maskImage) {
          currentPhoto.queryContainerImage().then(
              function(colorImage) {
                doBlendColorPop(colorImage, maskImage);

                imageContext.clearRect(0, 0, width, height);
                imageData = imageContext.createImageData(
                    colorImage.width, colorImage.height);
                imageData.data.set(colorImage.data);
                imageContext.putImageData(imageData, 0, 0);

                currentPhoto.clone().then(
                    function(photo) {
                      savePhoto = photo;
                      savePhoto.setContainerImage(colorImage).then(
                          function() {
                            statusElement.innerHTML = 'Redo Scribble Complete';
                          },
                          function(e) { statusElement.innerHTML = e.message; });
                    },
                    function(e) { statusElement.innerHTML = e.message; });
              },
              function(e) { statusElement.innerHTML = e.message; });
        },
        function(e) { statusElement.innerHTML = e.message; });
  };
}
