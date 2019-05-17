// @Author: Mursalimov Eduard

#define FNULL     (0.0f)

class CRegulator {
protected:
	float err;
	float last_err;

public:
	CRegulator() {};
	~CRegulator() {};

	/* Override */
	virtual float calculate(float err_val, float delta_t) = 0;	
	
	/* Override */
	virtual float calculate(float current_val, float target_val, float delta_t) = 0;	
};

class C_PID_Regulator : public CRegulator {
private:
	float kP, kI, kD;
	float int_err;
	float limit_integral;

	float _fabs(const float x) 
	{
	    return (x < FNULL) ? -x : x; 
	}

public:
	C_PID_Regulator(const float kp, const float ki, const float kd, const float limit_integral_err=100.0f) :
		kP(kp), kI(ki), kD(kd), limit_integral(limit_integral_err) {};
	~C_PID_Regulator() {};

	/* Override */
	float calculate(float err_val, float delta_t) {
		float result = 0.0f;
		last_err = err;
		err = err_val;
		result = kP * err + kI*int_err + kD * (err - last_err) / delta_t;
		if (_fabs(result) < limit_integral)
			int_err += err*delta_t;
		return result;
	}

	/* Override */
	float calculate(float current_val, float target_val, float delta_t) {
		float result = 0.0f;
		last_err = err;
		err = target_val - current_val;
		result = kP * err + kI*int_err + kD * (err - last_err) / delta_t;
		if(_fabs(result) < limit_integral)
			int_err += err*delta_t;
		return result;
	}
};
