using UnityEngine;
using System.Collections;

public class PlayControl : MonoBehaviour {
    public float speed = 150;
    private float m_fCameraFieldView = 0;
    private Vector3 m_vt3BornPosition;
    private Quaternion m_qtnBornRatation;

	// Use this for initialization
	void Start () {
        m_fCameraFieldView = Camera.main.fieldOfView;
        m_vt3BornPosition = transform.position;
        m_qtnBornRatation = transform.rotation;
	}
	
	// Update is called once per frame
	void Update () {
        //esc 退出应用程序
	    if(Input.GetKeyDown(KeyCode.Escape))
        {
            Application.Quit();
        }

        //摄像机缩放
        if (Input.GetAxis("Mouse ScrollWheel") < 0)
        {
            ZoomInOut(false);
        }
        if (Input.GetAxis("Mouse ScrollWheel") > 0)
        {
            ZoomInOut(true);
        }

        //旋转控制
        if(Input.GetMouseButton(0))
        {
            float fDirX = Input.GetAxis("Mouse Y");
            float fDirY = Input.GetAxis("Mouse X");
            TargetRotate(fDirX, fDirY);
        }

        //重置
        if(Input.GetKeyDown(KeyCode.Space))
        {
            Reset();
        }

	}

    public void Reset()
    {
        Camera.main.fieldOfView = m_fCameraFieldView;
        transform.position = m_vt3BornPosition;
        transform.rotation = m_qtnBornRatation;
    }

    public void ZoomInOut(bool bZoomIn)
    {
        if(bZoomIn)     //放大
        {
            if (Camera.main.fieldOfView > 2)
            {
                Camera.main.fieldOfView -= 2;
            }
        }
        else  //缩小
        {
            if (Camera.main.fieldOfView <= 100)
            {
                Camera.main.fieldOfView += 2;
            }
        }
        
    }


    public void TargetRotate(float fDirX, float fDirY)
    {
        float x = 0;
//        float x = fDirX * Time.deltaTime * speed;
        float y = fDirY * Time.deltaTime * speed*-1;
        
        transform.Rotate(new Vector3(x, y, 0), Space.World);
    }

}


