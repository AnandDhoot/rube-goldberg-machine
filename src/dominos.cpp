/*
* Copyright (c) 2006-2009 Erin Catto http://www.box2d.org
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

/* 
 * Base code for CS 251 Software Systems Lab Final Project Group 21 - Vulcans!
 * Department of Computer Science and Engineering, IIT Bombay
 * 
 */
 #include "cs251_base.hpp"
#include "render.hpp"
#include <cmath>
#ifdef __APPLE__
  #include <GLUT/glut.h>
#else
  #include "GL/freeglut.h"
#endif

#include <cstring>
using namespace std;

#include "dominos.hpp"
#include <vector>
#include <iostream>

float posx_cannon = 50.0f;
float gap_cannon = 16.0f;
/**
 * \class Explosion
 *
 * \brief Class for explosion of particles as well as cannon balls
 *
 * This class creates the particles of explosion and assigns them
 * velocities when they have to explode. This class is also used to 
 * create the cannon balls.
 *
 * \author (Anand , Maulik, Anchit) 
 *
 * $Date: 1/11/2014  $
 *
 */
class Explosion 
{
  public:
  b2Body* body;   ///< A body
  b2FixtureDef fd;  ///< A fixture to the body
  b2World* world; ///< A pointer to the world
  int iscanon;    ///< A flag to check the type of object represented by the object
  vector<b2Body*> v;///< A vector of all the particles which are going to blast away 
  int numRays;    ///< The number of particles 
  float x,y;    ///< The x,y coordinates of the particles
  
  /** Creates the cannon balls 
   */
  void explodecanon()
  {
    {
      b2BodyDef bd;
      bd.type = b2_dynamicBody;
      bd.fixedRotation = true; // rotation not necessary
      bd.bullet = true; // prevent tunneling at high speed
      //bd.linearDamping = 1; // drag due to moving through air
      bd.gravityScale = 1.0; // ignore gravity
      //change x coor here
      bd.position.Set(posx_cannon + 1.5,0); // start at blast center
      //bd.linearVelocity = 30 * rayDir;
      b2Body* bod = world->CreateBody( &bd );
      v.push_back(bod);
      b2CircleShape circleShape;
      circleShape.m_radius = 0.5; // very small
      b2FixtureDef fd;
      fd.shape = &circleShape;
      fd.density = 0.01 ; // very high - shared across all particles
      fd.friction = 0; // friction not necessary
      fd.restitution = 0; // high restitution to reflect off obstacles
      //fd.filter.groupIndex = -1; // particles should not collide with each other
      bod->CreateFixture( &fd );
    }
    {
      b2BodyDef bd;
      bd.type = b2_dynamicBody;
      bd.fixedRotation = true; // rotation not necessary
      bd.bullet = true; // prevent tunneling at high speed
      //bd.linearDamping = 1; // drag due to moving through air
      bd.gravityScale = 1.0; // ignore gravity
      //change x coor here
      bd.position.Set(posx_cannon + 1 * gap_cannon + 1.5,0); // start at blast center
      //bd.linearVelocity = 30 * rayDir;
      b2Body* bod = world->CreateBody( &bd );
      v.push_back(bod);
      b2CircleShape circleShape;
      circleShape.m_radius = 0.5; // very small
      b2FixtureDef fd;
      fd.shape = &circleShape;
      fd.density = 0.01 ; // very high - shared across all particles
      fd.friction = 0; // friction not necessary
      fd.restitution = 0.0f; // high restitution to reflect off obstacles
      //fd.filter.groupIndex = -1; // particles should not collide with each other
      bod->CreateFixture( &fd );
    }
  }
  
  /**Creates the particles which will fly off in all directions 
   * in the event of an explosion
   */
  void explode()
  {
    for (int i = 0; i < numRays; i++) 
    {
      b2BodyDef bd;
      bd.type = b2_dynamicBody;
      bd.fixedRotation = true; // rotation not necessary
      bd.bullet = true; // prevent tunneling at high speed
      //bd.linearDamping = 1; // drag due to moving through air
      bd.gravityScale = 0.0; // ignore gravity
      bd.position.Set(x,y+10); // start at blast center
      //bd.linearVelocity = 30 * rayDir;
    
      b2Body* bod = world->CreateBody( &bd );
      v.push_back(bod);
      b2FixtureDef fd;
      b2CircleShape circleShape;
      circleShape.m_radius = 0.3; // very small
      fd.shape = &circleShape;
      fd.density = 60 / (float)numRays; // very high - shared across all particles
      fd.friction = 0; // friction not necessary
      fd.restitution = 0.99f; // high restitution to reflect off obstacles
      fd.filter.groupIndex = -3; // particles should not collide with each other
      bod->CreateFixture( &fd );
    }
  }
  
  /**Constructor for Explosion
   * Creates the cannon particles or the blasting particles depending on
   * an argument passed while creating the object
   */
  Explosion(int num,float xc,float yc,b2World* m_world,int flag)
  {
    b2BodyDef bd;
    numRays=num;
    x=xc;y=yc;
    world=m_world;
    iscanon=flag;
    if(iscanon==1)
    {
      b2BodyDef bd;
      numRays=num;
      x=xc;y=yc;
      world=m_world;
      bd.type = b2_staticBody;
      bd.gravityScale = 0; // ignore gravity
      bd.position.Set(x,y);
      body = m_world->CreateBody( &bd );

      b2CircleShape circleShape;
      circleShape.m_radius = .12; // very small

      b2FixtureDef fd;
      fd.shape = &circleShape;
      fd.density = 0 / (float)numRays; // very high - shared across all particles
      fd.filter.groupIndex = -2; 
      body->CreateFixture( &fd );
      body->SetUserData(this);
      explodecanon();
    }
    else{
      bd.type = b2_dynamicBody;
      bd.gravityScale = 0; // ignore gravity
      bd.position.Set(x,y);
      body = m_world->CreateBody( &bd );

      b2CircleShape circleShape;
      circleShape.m_radius = .212; // very small

      b2FixtureDef fd;
      fd.shape = &circleShape;
      fd.density = 0 / (float)numRays; // very high - shared across all particles
      fd.filter.groupIndex = -1; 
      body->CreateFixture( &fd );
      body->SetUserData(this);
      explode();
    }
  }
  /**assigns velocity to each of the particles so that they 
   * look like they have started flying off in different directions
   */
  void start()
  {
    if(iscanon==1)
    {
      b2Vec2 rayDir( 9, 30 );
      v[0]->SetLinearVelocity(3*rayDir);
      v[0]->ResetMassData();
      b2Vec2 rayDir1(-9, 30 );
      v[1]->SetLinearVelocity(3*rayDir1);
      v[1]->ResetMassData();
    }
    else if(iscanon==2)
    {
      for(unsigned int i=0;i<v.size();i++)
      {
        float angle = (((i*180.0 / numRays)) * 3.14159292/180)+55.1;
        b2Vec2 rayDir( sinf(angle), cosf(angle) );
        v[i]->SetLinearVelocity(30*rayDir);
      }
      body->SetLinearVelocity(b2Vec2(0,50));
    }
    else
    {
      for(unsigned int i=0;i<v.size();i++)
      {
		float angle = (((i*180.0 / numRays)) * 3.14159292/180)+55.1;
        b2Vec2 rayDir( sinf(angle), cosf(angle) );
        v[i]->SetLinearVelocity(30*rayDir);
      }
      body->SetLinearVelocity(b2Vec2(0,50));
    }
  }

};

/**\brief A Class to check for the event of a contact/collision
 * 
 * This class listens for the type of objects in a collision 
 * and calls the appropriate explosion function
 */
class MyContactListener : public b2ContactListener
{
	void BeginContact(b2Contact* contact) 
	{
		//check if fixture A was a ball
		void* bodyUserData = contact->GetFixtureA()->GetBody()->GetUserData();
		void* bodyUserData1 = contact->GetFixtureB()->GetBody()->GetUserData();
		if ( bodyUserData )
		{
			static_cast<Explosion*>( bodyUserData )->start();
		}
		//check if fixture B was a ball
		bodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
		if ( bodyUserData1 )
		{
			static_cast<Explosion*>( bodyUserData1 )->start();
		}
	}
};

/** An instance of a contact listener is created
 */
MyContactListener myContactListenerInstance;


  b2Body* body;   ///< A body
  b2FixtureDef fd;  ///< A fixture to the body
  b2World* world; ///< A pointer to the world
  int iscanon;    ///< A flag to check the type of object represented by the object
  vector<b2Body*> v;///< A vector of all the particles which are going to blast away 
  int numRays;    ///< The number of particles 
  float x,y;    ///< The x,y coordinates of the particles
  
  /** Creates static stoppers
   */
  void makeStopper(float posx, float posy, b2World* m_world)
  {
    {
    	b2PolygonShape shape;
		shape.SetAsBox(0.15f, 0.15f);
		b2BodyDef bd;
		bd.position.Set(posx, posy);
		b2Body* ground = m_world->CreateBody(&bd);
		ground->CreateFixture(&shape, 0.0f);
    }
  }

namespace cs251
{
  /**  The is the constructor 
   * This is the documentation block for the constructor.
   */ 
  dominos_t::dominos_t()
    {
      //Ground
      /*! \var b1 
       * \brief pointer to the body ground 
       */ 

	    float posx_dominos = -35.5f;
	    float posy_dominos = 2.0f;

	    b2Body* b1;  
	    {
			b2EdgeShape shape; 
			shape.Set(b2Vec2(-90.0f, 0.0f), b2Vec2(90.0f, 0.0f));
			b2BodyDef bd; 
			b1 = m_world->CreateBody(&bd); 
			b1->CreateFixture(&shape, 0.0f);
	    }


		//The newtons pendulum that knocks the dominos off
		{
			float posx = posx_dominos - 9.0f + 0.5f;
			float posy = posy_dominos - 0.5f;

			for(int i=0;i<6;i++)
			{
				// The base of the pendulums
				b2Body* b2;
				{
					b2PolygonShape shape;
					shape.SetAsBox(0.25f, 0.5f);

					b2BodyDef bd;
					bd.position.Set(posx + 1.5f*i, posy - 1.0f);
					b2 = m_world->CreateBody(&bd);
					b2->CreateFixture(&shape, 5.0f);
				}
				b2Body* b4;
				{
					b2CircleShape shape;
					shape.m_radius = 0.77;
					b2BodyDef bd;
					bd.type = b2_dynamicBody;
					if(i==0)
						bd.position.Set(posx - 3.5f, posy + 4.5f);
					else
						bd.position.Set(posx + 1.5*i, posy + 1.0f);
					b4 = m_world->CreateBody(&bd);
					b4->CreateFixture(&shape, 2.0f);
				}
				b2RevoluteJointDef jd;
				b2Vec2 anchor;
				anchor.Set(posx + 1.5*i, posy + 4.5f);
				jd.Initialize(b2, b4, anchor);
				m_world->CreateJoint(&jd);
	        }
	    }

	    // The four shelfs and the four balls on top of them
		float posx_shelfs = -18.0f;
		float posy_shelfs = 9.0f;
		{
			float posy_shelf1 = posy_shelfs;
			float posy_shelf2 = posy_shelfs + 3.8;
			float posy_shelf3 = posy_shelfs + 11.1;
			float posy_shelf4 = posy_shelfs + 16.0;

	        // horizontal shelfs
	        {
				float posx = posx_shelfs;
				//         float posy = posy_shelfs;
				b2PolygonShape shape;
				{
					shape.SetAsBox(11.0f, 0.25f);
					{           //the first bottom platforms
						b2BodyDef bd;
						bd.position.Set(posx, posy_shelf1);
						b2Body* ground = m_world->CreateBody(&bd);
						ground->CreateFixture(&shape, 0.0f);
					}
					{           //the second bottom platforms
						b2BodyDef bd;
						bd.position.Set(posx, posy_shelf2);
						b2Body* ground = m_world->CreateBody(&bd);
						ground->CreateFixture(&shape, 0.0f);
					}
	        	}
        		{
					shape.SetAsBox(2.5f, 0.25f);
					b2BodyDef bd;
					bd.position.Set(posx + 8.3f, posy_shelf3);
					b2Body* ground = m_world->CreateBody(&bd);
					ground->CreateFixture(&shape, 0.0f);
    	        }
	            {
					shape.SetAsBox(13.2f, 0.25f);
					b2BodyDef bd;
					bd.position.Set(posx + 2.0f, posy_shelf4);
					b2Body* ground = m_world->CreateBody(&bd);
					ground->CreateFixture(&shape, 0.0f);
		        }
				// Stoppers
				// Hinge 1
				makeStopper(posx - 13.0f, posy_shelf2 - 2.4f, m_world);
				makeStopper(posx - 13.0f, posy_shelf2 + 1.0f, m_world);
				// Hinge 2
				makeStopper(posx + 12.8f, posy_shelf2 - 2.4f, m_world);
				makeStopper(posx + 12.8f, posy_shelf2 - 6.4f, m_world);
				// Hinge 3
				makeStopper(posx + 12.6f, posy_shelf2 + 1.5f, m_world);
				makeStopper(posx + 12.6f, posy_shelf2 + 8.5f, m_world);
				// Hinge 4
				makeStopper(posx - 13.1f, posy_shelf2 + 5.5f, m_world);
				makeStopper(posx - 13.1f, posy_shelf2 + 14.0f, m_world);
			}
	        float posx_hinge_left = posx_shelfs - 12.0f;
	        float posx_hinge_right = posx_shelfs + 12.0f - 0.3f;

        //The hinged joints
      {         //hinge 1
        b2PolygonShape shape;
        shape.SetAsBox(0.2f, 2.6f);

        b2BodyDef bd;
        bd.position.Set(posx_hinge_left, posy_shelfs + 1.3f);
        bd.type = b2_dynamicBody;
        b2Body* body = m_world->CreateBody(&bd);
        b2FixtureDef *fd = new b2FixtureDef;
        fd->density = 8.0f;
        fd->shape = new b2PolygonShape;
        fd->shape = &shape;
        body->CreateFixture(fd);

        b2PolygonShape shape2;
        shape2.SetAsBox(2.0f, .2f);
        b2BodyDef bd2;
        bd2.position.Set(posx_hinge_left, posy_shelfs + 3.0f);
        b2Body* body2 = m_world->CreateBody(&bd2);

        b2RevoluteJointDef jointDef;
        jointDef.bodyA = body;
        jointDef.bodyB = body2;
        jointDef.localAnchorA.Set(0,0);
        jointDef.localAnchorB.Set(0,0);
        jointDef.collideConnected = false;
        m_world->CreateJoint(&jointDef);
        }
        {       //hinge 2
        b2PolygonShape shape;
        shape.SetAsBox(0.2f, 2.6f);

        b2BodyDef bd;
        bd.position.Set(posx_hinge_right + 0.0f,  posy_shelfs - 2.0f);
        bd.type = b2_dynamicBody;
        b2Body* body = m_world->CreateBody(&bd);
        b2FixtureDef *fd = new b2FixtureDef;
        fd->density = 8.0f;
        fd->shape = new b2PolygonShape;
        fd->shape = &shape;
        body->CreateFixture(fd);

        b2PolygonShape shape2;
        shape2.SetAsBox(2.0f, .2f);
        b2BodyDef bd2;
        bd2.position.Set(posx_hinge_right + 0.0f,  posy_shelfs - 0.7f);
        b2Body* body2 = m_world->CreateBody(&bd2);

        b2RevoluteJointDef jointDef;
        jointDef.bodyA = body;
        jointDef.bodyB = body2;
        jointDef.localAnchorA.Set(0,0);
        jointDef.localAnchorB.Set(0,0);
        jointDef.collideConnected = false;
        m_world->CreateJoint(&jointDef);
        }
        //right up
        {     //hinge 3
        b2PolygonShape shape;
        shape.SetAsBox(0.2f, 4.2f);

        b2BodyDef bd;
        bd.position.Set(posx_hinge_right + 0.0f,  posy_shelfs + 17.7f);
        bd.type = b2_dynamicBody;
        b2Body* body = m_world->CreateBody(&bd);
        b2FixtureDef *fd = new b2FixtureDef;
        fd->density = 8.0f;
        fd->shape = new b2PolygonShape;
        fd->shape = &shape;
        body->CreateFixture(fd);

        b2PolygonShape shape2;
        shape2.SetAsBox(2.0f, .2f);
        b2BodyDef bd2;
        bd2.position.Set(posx_hinge_right + 0.0f,  posy_shelfs + 8.7f);
        b2Body* body2 = m_world->CreateBody(&bd2);

        b2RevoluteJointDef jointDef;
        jointDef.bodyA = body;
        jointDef.bodyB = body2;
        jointDef.localAnchorA.Set(0,0);
        jointDef.localAnchorB.Set(0,0);
        jointDef.collideConnected = false;
        m_world->CreateJoint(&jointDef);
        }
        {     //hinge 4
        b2PolygonShape shape;
        shape.SetAsBox(0.2f, 5.2f); //plank

        b2BodyDef bd;
        bd.position.Set(posx_hinge_left,  posy_shelfs + 13.7f);
        bd.type = b2_dynamicBody;
        b2Body* body = m_world->CreateBody(&bd);
        b2FixtureDef *fd = new b2FixtureDef;
        fd->density = 8.0f;
        fd->shape = new b2PolygonShape;
        fd->shape = &shape;
        body->CreateFixture(fd);

        b2PolygonShape shape2;    //mid point
        shape2.SetAsBox(2.0f, 0.2f);
        b2BodyDef bd2;
        bd2.position.Set(posx_hinge_left, posy_shelfs + 13.7f);
        b2Body* body2 = m_world->CreateBody(&bd2);

        b2RevoluteJointDef jointDef;
        jointDef.bodyA = body;
        jointDef.bodyB = body2;
        jointDef.localAnchorA.Set(0,0);
        jointDef.localAnchorB.Set(0,0);
        jointDef.collideConnected = false;
        m_world->CreateJoint(&jointDef);
        }
        float posx_ball_left = posx_shelfs + 10.6f;
        float posx_ball_right = posx_shelfs - 11.0f;
        //The balls
      {     //ball 1
        b2Body* sbody;
        b2CircleShape circle;
        circle.m_radius = 0.7;

        b2FixtureDef ballfd;
        ballfd.shape = &circle;
        ballfd.density = 3.0f;
        ballfd.friction = 0.01f;
        ballfd.restitution = 1.0f;
        b2BodyDef ballbd;
        ballbd.type = b2_dynamicBody;
        ballbd.position.Set(posx_ball_left, posy_shelf1 + 0.5f);
        sbody = m_world->CreateBody(&ballbd);
        sbody->CreateFixture(&ballfd);
        }
        {   //ball 2
        b2Body* sbody;
        b2CircleShape circle;
        circle.m_radius = 0.7;

        b2FixtureDef ballfd;
        ballfd.shape = &circle;
        ballfd.density = 2.0f;
        ballfd.friction = 0.01f;
        ballfd.restitution = 1.0f;
        b2BodyDef ballbd;
        ballbd.type = b2_dynamicBody;
        ballbd.position.Set(posx_ball_right, posy_shelf2 + 0.5f);
        sbody = m_world->CreateBody(&ballbd);
        sbody->CreateFixture(&ballfd);
        }
        { //ball 3
        b2Body* sbody;
        b2CircleShape circle;
        circle.m_radius = 0.7;

        b2FixtureDef ballfd;
        ballfd.shape = &circle;
        ballfd.density = 5.0f;
        ballfd.friction = 0.0f;
        ballfd.restitution = -10.0f;
        b2BodyDef ballbd;
        ballbd.type = b2_dynamicBody;
        ballbd.position.Set(posx_ball_left, posy_shelf3 + 0.5f);
        sbody = m_world->CreateBody(&ballbd);
        sbody->CreateFixture(&ballfd);
        }
        { //ball 4
        b2Body* sbody;
        b2CircleShape circle;
        circle.m_radius = 0.7;

        b2FixtureDef ballfd;
        ballfd.shape = &circle;
        ballfd.density = 2.0f;
        ballfd.friction = 0.0f;
        ballfd.restitution = -10.0f;
        b2BodyDef ballbd;
        ballbd.type = b2_dynamicBody;
        ballbd.position.Set(posx_ball_left + 4.0f, posy_shelf4 + 0.5f);
        sbody = m_world->CreateBody(&ballbd);
        sbody->CreateFixture(&ballfd);
        }
    }

		//Dominos
		{
			float posx = posx_dominos;
			float posy = posy_dominos;
			//the ones with the increasing sizes
			for (int i = 0; i < 10; ++i)
			{
				b2PolygonShape shape;
				shape.SetAsBox(0.2f, 0.2f*(i+1));

				b2FixtureDef fd;
				fd.shape = &shape;
				fd.density = 20.0f;
				fd.friction = 0.1f;
				b2BodyDef bd;
				bd.type = b2_staticBody;
				bd.position.Set(posx + 15.0f + 1.5f * i, posy + i*0.2f - 1.8f);
				b2Body* body = m_world->CreateBody(&bd);
				body->CreateFixture(&fd);
			}
			//the ones with fixed sizes on top of the steps
			for (int i = 0; i < 10; ++i)
			{
				b2PolygonShape shape;
				shape.SetAsBox(0.1f, 1.5f);

				b2FixtureDef fd;
				fd.shape = &shape;
				fd.density = 70.0f;
				fd.friction = 0.1f;

				b2BodyDef bd;
				bd.type = b2_dynamicBody;
				bd.position.Set(posx + 15.0f + 1.5f * i, posy + 3.4f);
				b2Body* body = m_world->CreateBody(&bd);
				body->CreateFixture(&fd);
	      	}
			//the ones with the fixed sizes and on the ground
			for (int i = 0; i < 10; ++i)
			{
				b2PolygonShape shape;
				shape.SetAsBox(0.1f, 1.5f);       
				b2FixtureDef fd;
				fd.shape = &shape;
				fd.density = 70.0f;
				fd.friction = 0.1f;

				b2BodyDef bd;
				bd.type = b2_dynamicBody;
				bd.position.Set(posx + 1.5f * i, posy);
				b2Body* body = m_world->CreateBody(&bd);
				body->CreateFixture(&fd);
			}
			//the ones on the top platform
			for (int i = 0; i < 15; ++i)
			{
				b2PolygonShape shape;
				shape.SetAsBox(0.1f, 1.7f);

				b2FixtureDef fd;
				fd.shape = &shape;
				fd.density = 10.0f;
				fd.friction = 0.1f;

				b2BodyDef bd;
				bd.type = b2_dynamicBody;
				bd.position.Set(posx_shelfs - 11.0f + 1.3f * i, posy_shelfs + 19.0f);
				b2Body* body = m_world->CreateBody(&bd);
				body->CreateFixture(&fd);
			}
			//the ones on the top platform - layer two
			for (int i = 0; i < 15; ++i)
			{
				b2PolygonShape shape;
				shape.SetAsBox(0.1f, 1.7f);

				b2FixtureDef fd;
				fd.shape = &shape;
				fd.density = 10.0f;
				fd.friction = 0.1f;

				b2BodyDef bd;
				bd.type = b2_dynamicBody;
				bd.position.Set(posx_shelfs - 11.0f + 1.3f * i, posy_shelfs + 25.0f);
				b2Body* body = m_world->CreateBody(&bd);
				body->CreateFixture(&fd);
			}
			//the ones on the top platform - layer three
			for (int i = 0; i < 15; ++i)
			{
				b2PolygonShape shape;
				shape.SetAsBox(0.1f, 1.7f);

				b2FixtureDef fd;
				fd.shape = &shape;
				fd.density = 10.0f;
				fd.friction = 0.1f;

				b2BodyDef bd;
				bd.type = b2_dynamicBody;
				bd.position.Set(posx_shelfs - 11.0f + 1.3f * i, posy_shelfs + 35.0f);
				b2Body* body = m_world->CreateBody(&bd);
				body->CreateFixture(&fd);
			}
		}

		//The pulley system
		float posx_pulley = 0.0f;
		float posy_pulley = 20.0f;
		{
			float posx = posx_pulley;
			float posy = posy_pulley;
			float density_plane = 5.0f;

			b2BodyDef *bd = new b2BodyDef;
			bd->type = b2_dynamicBody;
			bd->position.Set(posx, posy);
			bd->fixedRotation = true;

			//The open box
			b2FixtureDef *fd1 = new b2FixtureDef;
			fd1->density = density_plane;
			fd1->friction = 0.5;
			fd1->restitution = 0.f;
			fd1->shape = new b2PolygonShape;
			b2PolygonShape bs1;
			bs1.SetAsBox(2,0.2, b2Vec2(0.f,-1.9f), 0);
			fd1->shape = &bs1;

			b2FixtureDef *fd2 = new b2FixtureDef;
			fd2->density = density_plane;
			fd2->friction = 0.5;
			fd2->restitution = 0.f;
			fd2->shape = new b2PolygonShape;
			b2PolygonShape bs2;
			bs2.SetAsBox(0.2,2, b2Vec2(2.0f,0.f), 0);
			fd2->shape = &bs2;

			b2FixtureDef *fd3 = new b2FixtureDef;
			fd3->density = density_plane;
			fd3->friction = 0.5;
			fd3->restitution = 0.f;
			fd3->shape = new b2PolygonShape;
			b2PolygonShape bs3;
			bs3.SetAsBox(0.2,2, b2Vec2(-2.0f,0.f), 0);
			fd3->shape = &bs3;

			b2Body* box1 = m_world->CreateBody(bd);
			box1->CreateFixture(fd1);
			box1->CreateFixture(fd2);
			box1->CreateFixture(fd3);

			//The bar
			bd->position.Set(posx + 10.0f, posy); 
			//  fd1.SetAsBox(15.0f, 0.2f);
			fd1->density = 3 * density_plane;   
			b2Body* box2 = m_world->CreateBody(bd);
			box2->CreateFixture(fd1);

			// The pulley joint
			b2PulleyJointDef* myjoint = new b2PulleyJointDef();
			b2Vec2 worldAnchorOnBody1(posx, posy); ///< Anchor point on body 1 in world axis
			b2Vec2 worldAnchorOnBody2(posx + 10.0f, posy); /// Anchor point on body 2 in world axis
			b2Vec2 worldAnchorGround1(posx, posy + 5.0f); // Anchor point for ground 1 in world axis
			b2Vec2 worldAnchorGround2(posx + 10.0f, posy + 5.0f); // Anchor point for ground 2 in world axis
			float32 ratio = 1.0f; // Define ratio
			myjoint->Initialize(box1, box2, worldAnchorGround1, worldAnchorGround2, box1->GetWorldCenter(), box2->GetWorldCenter(), ratio);
			m_world->CreateJoint(myjoint);
		}
		// Shelf to be activated by the pulley
		{
			float posx = posx_pulley + 19.0f;
			float posy = posy_pulley + 3.0f;
			{
				b2PolygonShape shape;
				shape.SetAsBox(5.0f, 0.25f);
				for(int i=0;i<4;i++)
				{
					b2BodyDef bd;
					bd.position.Set(posx, posy);
					b2Body* ground = m_world->CreateBody(&bd);
					ground->CreateFixture(&shape, 0.0f);
				}
			}
			{
				b2PolygonShape shape;
				shape.SetAsBox(0.25f, 1.2f);
				b2BodyDef bd;
				bd.position.Set(posx + 18.3f, posy + 1.0f);
				b2Body* ground = m_world->CreateBody(&bd);
				ground->CreateFixture(&shape, 0.0f);
			}
			// Plank
			{
				b2PolygonShape shape;
				shape.SetAsBox(3.0f, 0.2f);

				b2BodyDef bd;
				bd.position.Set(posx - 4.5f, posy + 2.0f);
				bd.type = b2_dynamicBody;
				b2Body* body = m_world->CreateBody(&bd);
				b2FixtureDef *fd = new b2FixtureDef;
				fd->density = 1.0f;
				fd->shape = new b2PolygonShape;
				fd->shape = &shape;
				body->CreateFixture(fd);
			}
			// Ball
			{
				b2Body* sbody;
				b2CircleShape circle;
				circle.m_radius = 0.7;

				b2FixtureDef ballfd;
				ballfd.shape = &circle;
				ballfd.density = 2.0f;
				ballfd.friction = 0.7f;
				ballfd.restitution = 0.3f;
			    b2BodyDef ballbd;
		        ballbd.type = b2_dynamicBody;
		        ballbd.position.Set(posx - 5.0f, posy + 3.0f);
		        sbody = m_world->CreateBody(&ballbd);
		        sbody->CreateFixture(&ballfd);
			}
		}
		// Plank to split linear flow of action
		{
			float posx = posx_pulley + 31.0f;
			float posy = posy_pulley + 3.0f;

			b2PolygonShape shape;
			shape.SetAsBox(5.5f, 0.25f);
			for(int i=0;i<4;i++)
			{
				b2BodyDef bd;
				bd.position.Set(posx + 0.5f, posy);
				b2Body* ground = m_world->CreateBody(&bd);
				ground->CreateFixture(&shape, 0.0f);
			}
			// Ball
			{
				b2Body* sbody;
				b2CircleShape circle;
				circle.m_radius = 0.7;

				b2FixtureDef ballfd;
				ballfd.shape = &circle;
				ballfd.density = 0.3f;
				ballfd.friction = 0.3f;
				ballfd.restitution = 0.3f;

				b2BodyDef ballbd;
				ballbd.type = b2_dynamicBody;
				ballbd.position.Set(posx - 5.0f, posy + 1.0f);
				sbody = m_world->CreateBody(&ballbd);
				sbody->CreateFixture(&ballfd);
			}
			Explosion* anar;
			for(int i=0;i<5;i++)
				anar=new Explosion(25, posx-3+i*2 , 2.0+posy-0.4 , m_world, 0);

			anar=new Explosion(2, 40.0 , 1.0 , m_world, 1);
			anar=new Explosion(30, 58.0 , 24.0 , m_world, 2);
			anar=new Explosion(30, 59.5 , 24.0 , m_world, 2);
			anar=new Explosion(30, 61.0 , 24.0 , m_world, 2);
			makeStopper(57.0, 25.2, m_world);
			makeStopper(62.0, 25.2, m_world);
			(anar->x)++;
			{
				b2PolygonShape shape;
				shape.SetAsBox(3.0f, 0.2f);

				b2BodyDef bd;
				bd.position.Set(59.5,22.0f);
				bd.type = b2_dynamicBody;
				bd.gravityScale=0.0;
				b2Body* body = m_world->CreateBody(&bd);
				b2FixtureDef *fd = new b2FixtureDef;
				fd->density = 1.0f;

				fd->shape = new b2PolygonShape;
				fd->shape = &shape;
				body->CreateFixture(fd);
			}
			m_world->SetContactListener(&myContactListenerInstance);
		}

	    // The steps
	    {
			int steps = 3;            //number of steps
			float slantEdgePosX = 26.0f;    //coordinates
			float slantEdgePosY = 2.0f;   //coordinates
			float direction = 1;        //one when top-left to bottom right ; -1 for top-right to bottom-left
			float horSize = 5.5f;       // width of plank's shadow on X-axis
			float vertSize = 4.0f ;     // height of plank's shadow on Y-axis
			float shiftRight = 4.5f;      // horizontal distance between two opposite facing planks
			float shiftUp = vertSize;     // vertical distance between two opposite facing planks
			float distance = 2*vertSize;    //distance between two same-side facing steps
			for(int i =0 ; i< steps ; i++)
			{
				b2EdgeShape shape;                                            //  line segment used to define the path.
				shape.Set(b2Vec2(slantEdgePosX-horSize/2, slantEdgePosY+vertSize/2*direction + i*distance), b2Vec2(slantEdgePosX+horSize/2, slantEdgePosY-vertSize/2*direction + i*distance));      // The two end points of the path are specified.
				b2BodyDef bd;                                                 //Used to define the body.
				b1 = m_world->CreateBody(&bd);                            // Body created in the Box2D world.
				b1->CreateFixture(&shape, 0.0f);                      // Static Body, so the fixture is created without changing its default values.
			}

			direction = -1;
			slantEdgePosX += shiftRight;
			slantEdgePosY += shiftUp;
			for(int i =0 ; i< steps-1 ; i++)
			{
				b2EdgeShape shape;                                            //  line segment used to define the path.
				shape.Set(b2Vec2(slantEdgePosX-horSize/2, slantEdgePosY+vertSize/2*direction + i*distance), b2Vec2(slantEdgePosX+horSize/2, slantEdgePosY-vertSize/2*direction + i*distance));      // The two end points of the path are specified.
				b2BodyDef bd;                                                 //Used to define the body.
				b1 = m_world->CreateBody(&bd);                            // Body created in the Box2D world.
				b1->CreateFixture(&shape, 0.0f);                      // Static Body, so the fixture is created without changing its default values.
			}
		}

		// Boundaries for the cannon between which cannonball would be contained
		{
		    {
				b2EdgeShape shape;                                            //  line segment used to define the path.
				shape.Set(b2Vec2(posx_cannon, 0.0f), b2Vec2(posx_cannon + 1.5, 2.5));      // The two end points of the path are specified.
				b2BodyDef bd;                                                 //Used to define the body.
				b1 = m_world->CreateBody(&bd);                            // Body created in the Box2D world.
				b1->CreateFixture(&shape, 0.0f);                      // Static Body, so the fixture is created without changing its default values.
		    }
		    {
				b2EdgeShape shape;                                            //  line segment used to define the path.
				shape.Set(b2Vec2(posx_cannon + 3.0f, 0.0), b2Vec2(posx_cannon + 4.5, 2.5));      // The two end points of the path are specified.
				b2BodyDef bd;                                                 //Used to define the body.
				b1 = m_world->CreateBody(&bd);                            // Body created in the Box2D world.
				b1->CreateFixture(&shape, 0.0f);                      // Static Body, so the fixture is created without changing its default values.
		    }
		    {
				b2EdgeShape shape;                                            //  line segment used to define the path.
				shape.Set(b2Vec2(posx_cannon + 3.0f+ gap_cannon, 0.0f), b2Vec2(posx_cannon + 1.5+ gap_cannon, 2.5));      // The two end points of the path are specified.
				b2BodyDef bd;                                                 //Used to define the body.
				b1 = m_world->CreateBody(&bd);                            // Body created in the Box2D world.
				b1->CreateFixture(&shape, 0.0f);                      // Static Body, so the fixture is created without changing its default values.
		    }
		    {
				b2EdgeShape shape;                                            //  line segment used to define the path.
				shape.Set(b2Vec2(posx_cannon + gap_cannon, 0.0), b2Vec2(posx_cannon + -1.5+ gap_cannon, 2.5));      // The two end points of the path are specified.
				b2BodyDef bd;                                                 //Used to define the body.
				b1 = m_world->CreateBody(&bd);                            // Body created in the Box2D world.
				b1->CreateFixture(&shape, 0.0f);                      // Static Body, so the fixture is created without changing its default values.
			}
		}



    //The conveyor belt
    float posx_conveyer = posx_shelfs - 9.0f;
    float posy_conveyer = posy_shelfs + 7.2f;
    {
      double conveyorleft = posx_conveyer;
      double conveyorup = posy_conveyer;
      double discRadius = 1.0;
      int links = 9;
      double speed = 4.0;
      b2Body* leftup;
      b2Body* rightup;
      b2Body* leftdown;
      b2Body* rightdown;
      leftup = NULL;
      rightup = NULL;
      leftdown = NULL;
      rightdown = NULL;
      double density = 25.0f;
      double friction = 0.5f;
      double restitution = 0.05f;
      //Upper part
      {
        b2BodyDef bd;
        bd.type = b2_dynamicBody;
        bd.position.Set(conveyorleft, conveyorup);
        b2Body* ground = m_world->CreateBody(&bd);

        b2PolygonShape chainpiece;
        chainpiece.SetAsBox(1, 0.2);

        b2FixtureDef chainfix;
        chainfix.density = density;
        chainfix.friction = friction;
        chainfix.restitution = restitution;
        chainfix.shape = &chainpiece;

        b2Body *lastLink = ground;
        b2RevoluteJointDef chainJoint;
        chainJoint.collideConnected = false;

        for(int32 i = 0; i < links; i++)
        {
          double cons = 2.0;
          b2BodyDef bd;
          bd.type = b2_dynamicBody;
          bd.position.Set(i*cons + conveyorleft, 0.0 + conveyorup);
          b2Body* nextLink = m_world->CreateBody(&bd);
          nextLink->CreateFixture(&chainfix);
          b2Vec2 anchor(i*cons - 1.0 + conveyorleft, 0.0 + conveyorup);
          if(i > 0)
          {
            chainJoint.Initialize(lastLink, nextLink, anchor);
            m_world->CreateJoint(&chainJoint);
          }
          lastLink = nextLink;
          rightdown = nextLink;
          if(i == 0)
          {
            leftdown = nextLink;
          }
        }
      }
      // Lower part
      {
        double shiftup = 2.0*discRadius;
        b2BodyDef bd;
        bd.type = b2_dynamicBody;
        bd.position.Set(conveyorleft, conveyorup + shiftup);
        b2Body* ground = m_world->CreateBody(&bd);

        b2PolygonShape chainpiece;
        chainpiece.SetAsBox(1, 0.2);
        
        b2FixtureDef chainfix;
        chainfix.density = density;
        chainfix.friction = friction;
        chainfix.restitution = restitution;
        chainfix.shape = &chainpiece;

        b2Body *lastLink = ground;
        b2RevoluteJointDef chainJoint;
        chainJoint.collideConnected = false;

        for(int32 i = 0; i < links; i++)
        {
          double cons = 2.0;
          b2BodyDef bd;
          bd.type = b2_dynamicBody;
          bd.position.Set(i*cons + conveyorleft, shiftup + conveyorup);
          b2Body* nextLink = m_world->CreateBody(&bd);
          nextLink->CreateFixture(&chainfix);
          b2Vec2 anchor(i*cons - 1.0 + conveyorleft, shiftup + conveyorup);
          if(i > 0)
          {
            chainJoint.Initialize(lastLink, nextLink, anchor);
            m_world->CreateJoint(&chainJoint);
          }
          lastLink = nextLink;
          rightup = nextLink;
          if(i == 0)
          {
            leftup = nextLink;
          }
        }
      }
      // Connect the upper and lower parts on left
      {
        b2BodyDef bd;
        bd.type = b2_dynamicBody;
        bd.position.Set(-1.0 + conveyorleft, conveyorup + 0.5);
        b2Body* ground = m_world->CreateBody(&bd);

        b2PolygonShape chainpiece;
        chainpiece.SetAsBox(0.2, 1.0, b2Vec2(0.0,0.0),0.0);

        b2FixtureDef chainfix;
        chainfix.density = 50;
        chainfix.friction = 0.5;
        chainfix.restitution = 0.5;
        chainfix.shape = &chainpiece;
        
        ground->CreateFixture(&chainfix);
        bd.position.Set(-1.0 + conveyorleft, conveyorup + 1.0);
        
        b2Body*groundnew = m_world->CreateBody(&bd);
        groundnew->CreateFixture(&chainfix);
        
        b2RevoluteJointDef chainJoint;
        chainJoint.collideConnected = false;
        b2Vec2 anchor( conveyorleft - 1.0, conveyorup);
        //Connecting the lower horizontal link and the lower vertical link
        chainJoint.Initialize(leftdown, ground, anchor);
        m_world->CreateJoint(&chainJoint);
        b2RevoluteJointDef chainJoint2;
        chainJoint2.collideConnected = false;
        b2Vec2 anchor2( conveyorleft - 1.0, 1.0 + conveyorup);
        //Connecting the two vertical links
        chainJoint2.Initialize(ground, groundnew, anchor2);
        m_world->CreateJoint(&chainJoint2);
        b2RevoluteJointDef chainJoint3;
        chainJoint3.collideConnected = false;
        b2Vec2 anchor3( conveyorleft - 1.0, 2.0 + conveyorup);
        //Connecting the upper vertical link and the top horizontal link
        chainJoint3.Initialize(groundnew, leftup, anchor3);
        m_world->CreateJoint(&chainJoint3);
      }
      // Connect the upper and lower parts on right
      {
        b2BodyDef bd;
        bd.type = b2_dynamicBody;
        bd.position.Set(2.0 * links - 1.0+ conveyorleft, conveyorup + 0.5);
        b2Body* ground = m_world->CreateBody(&bd);

        b2PolygonShape chainpiece;
        chainpiece.SetAsBox(0.2, 1, b2Vec2(0.0,0.0),0.0);

        b2FixtureDef chainfix;
        chainfix.density = 50;
        chainfix.friction = 0.5;
        chainfix.restitution = 0.5;
        chainfix.shape = &chainpiece;

        ground->CreateFixture(&chainfix);
        bd.position.Set(2.0* links - 1.0+ conveyorleft, conveyorup + 1.0);

        b2Body*groundnew = m_world->CreateBody(&bd);
        groundnew->CreateFixture(&chainfix);

        b2RevoluteJointDef chainJoint;
        chainJoint.collideConnected = false;
        b2Vec2 anchor( 2.0 * links - 1.0 + conveyorleft, 0.0 + conveyorup);
        //Connecting the lower horizontal link and the lower vertical link
        chainJoint.Initialize(rightdown, ground, anchor);
        m_world->CreateJoint(&chainJoint);
        b2RevoluteJointDef chainJoint2;
        chainJoint2.collideConnected = false;
        b2Vec2 anchor2( 2.0 * links - 1.0+ conveyorleft, 1.0 + conveyorup);
        //Connecting the two vertical links
        chainJoint2.Initialize(ground, groundnew, anchor2);
        m_world->CreateJoint(&chainJoint2);
        b2RevoluteJointDef chainJoint3;
        chainJoint3.collideConnected = false;
        b2Vec2 anchor3( 2.0 * links - 1.0+ conveyorleft, 2.0 + conveyorup);
        //Connecting the upper vertical link and the top horizontal link
        chainJoint3.Initialize(groundnew, rightup, anchor3);
        m_world->CreateJoint(&chainJoint3);
      }
      // Rotating high friction discs
      // The left most disc
      {
        b2BodyDef bd;
        bd.type = b2_kinematicBody;
        bd.position.Set(conveyorleft + 0.0, conveyorup + 1.0);
        bd.angle = 0;

        b2Body* body = m_world->CreateBody(&bd);
        b2CircleShape circle;
        circle.m_radius = discRadius;
        
        b2FixtureDef fd;
        fd.shape = &circle;
        fd.density = 100;
        fd.friction = 1;
        
        body->CreateFixture(&fd);
        body->SetAngularVelocity(speed);
      }
      //The right most disc
      {
        b2BodyDef bd;
        bd.type = b2_kinematicBody;
        bd.position.Set(links * 2.0 + conveyorleft + -2.0, conveyorup + 1.0);
        bd.angle = 0;

        b2Body* body = m_world->CreateBody(&bd);
        b2CircleShape circle;
        circle.m_radius = discRadius;

        b2FixtureDef fd;
        fd.shape = &circle;
        fd.density = 100;
        fd.friction = 1;

        body->CreateFixture(&fd);
        body->SetAngularVelocity(speed);
      }
      //The second disc
      {
        b2BodyDef bd;
        bd.type = b2_kinematicBody;
        bd.position.Set(links/2 * 2.0 + conveyorleft + -2.0, conveyorup + 1.0);
        bd.angle = 0;

        b2Body* body = m_world->CreateBody(&bd);
        b2CircleShape circle;
        circle.m_radius = discRadius;

        b2FixtureDef fd;
        fd.shape = &circle;
        fd.density = 100;
        fd.friction = 1;

        body->CreateFixture(&fd);
        body->SetAngularVelocity(speed);
      }
      //The third disc
      {
        b2BodyDef bd;
        bd.type = b2_kinematicBody;
        bd.position.Set(3 * links/4 * 2.0 + conveyorleft + -2.0, conveyorup + 1.0);
        bd.angle = 0;

        b2Body* body = m_world->CreateBody(&bd);
        b2CircleShape circle;
        circle.m_radius = discRadius;

        b2FixtureDef fd;
        fd.shape = &circle;
        fd.density = 100;
        fd.friction = 1;

        body->CreateFixture(&fd);
        body->SetAngularVelocity(speed);
      }
    }
  }
  sim_t *sim = new sim_t("Dominos", dominos_t::create);
}
