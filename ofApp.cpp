#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(30);
	ofBackground(239);
	ofSetWindowTitle("Insta");

	this->fbo.allocate(ofGetWidth(), ofGetHeight());
	this->font.loadFont("fonts/Kazesawa-Bold.ttf", 50, true, false, true);

	this->box2d.init();
	this->box2d.setGravity(0, 50);
	this->box2d.createBounds();
	this->box2d.setFPS(30);

	for (int i = 0; i < 39 * 2; i++) {

		shared_ptr<ofxBox2dCircle> circle = shared_ptr<ofxBox2dCircle>(new ofxBox2dCircle);
		circle.get()->setPhysics(3.0, 0.5, 0.1);
		circle.get()->setup(this->box2d.getWorld(), ofRandom(ofGetWidth()), ofRandom(ofGetHeight()), ofRandom(9, 39));
		this->circles.push_back(circle);
	}
}

//--------------------------------------------------------------
void ofApp::update() {
	
	ofSeedRandom(39);

	this->sensor_point = this->get_accelerometer_3d();
	this->box2d.setGravity(this->sensor_point.x * 30, this->sensor_point.y * -30);
	this->box2d.update();

	this->fbo.begin();
	ofClear(0);

	ofSetColor(39, 139);
	this->font.drawString("X = " + std::to_string(this->sensor_point.x), 0, 100);
	this->font.drawString("Y = " + std::to_string(this->sensor_point.y), 0, 200);
	this->font.drawString("Z = " + std::to_string(this->sensor_point.z), 0, 300);

	for (int i = 0; i < this->circles.size(); i++) {

		ofColor circle_color;
		circle_color.setHsb(ofRandom(255), 239, 239);
		ofSetColor(circle_color);

		ofPushMatrix();
		ofTranslate(this->circles[i]->getPosition());

		ofBeginShape();
		for (int deg = 0; deg <= 360; deg++) {

			ofVertex(this->circles[i]->getRadius() * cos(deg * DEG_TO_RAD), this->circles[i]->getRadius() * sin(deg * DEG_TO_RAD));
		}
		ofEndShape();

		ofPopMatrix();
	}

	this->fbo.end();
}

//--------------------------------------------------------------
void ofApp::draw() {

	this->fbo.draw(0, 0);
}

//--------------------------------------------------------------
ofPoint ofApp::get_accelerometer_3d() {

	ISensorManager* sensor_manager = nullptr;
	ISensorCollection* sensor_collection = nullptr;
	ISensor* sensor = nullptr;
	ISensorDataReport* data = nullptr;

	if (!SUCCEEDED(::CoInitializeEx(NULL, COINIT_MULTITHREADED))) {

		return ofPoint();
	}

	if (!SUCCEEDED(::CoCreateInstance(CLSID_SensorManager, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&sensor_manager)))) {

		return ofPoint();
	}

	if (!SUCCEEDED(sensor_manager->GetSensorsByCategory(SENSOR_TYPE_ACCELEROMETER_3D, &sensor_collection))) {

		sensor_manager->Release();
		return ofPoint();
	}

	if (!SUCCEEDED(sensor_collection->GetAt(0, &sensor))) {

		sensor_collection->Release();
		sensor_manager->Release();
		return ofPoint();
	}

	if (!SUCCEEDED(sensor->GetData(&data))) {

		sensor->Release();
		sensor_collection->Release();
		sensor_manager->Release();
		return ofPoint();
	}

	PROPVARIANT x = {};
	if (!SUCCEEDED(data->GetSensorValue(SENSOR_DATA_TYPE_ACCELERATION_X_G, &x))) {

		data->Release();
		sensor->Release();
		sensor_collection->Release();
		sensor_manager->Release();
		return ofPoint();
	}

	PROPVARIANT y = {};
	if (!SUCCEEDED(data->GetSensorValue(SENSOR_DATA_TYPE_ACCELERATION_Y_G, &y))) {

		data->Release();
		sensor->Release();
		sensor_collection->Release();
		sensor_manager->Release();
		return ofPoint();
	}

	PROPVARIANT z = {};
	if (!SUCCEEDED(data->GetSensorValue(SENSOR_DATA_TYPE_ACCELERATION_Z_G, &z))) {

		data->Release();
		sensor->Release();
		sensor_collection->Release();
		sensor_manager->Release();
		return ofPoint();
	}

	data->Release();
	sensor->Release();
	sensor_collection->Release();
	sensor_manager->Release();

	return ofPoint(x.dblVal, y.dblVal, z.dblVal);
}

//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(1280, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}